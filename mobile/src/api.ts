export type RobotMode = "autonomous" | "manual";

export type RobotStatus = {
  mode?: RobotMode;
  motor?: string;
  server?: string;
};

export type SensorData = {
  front?: boolean | number;
  side?: boolean | number;
};

export type LedData = {
  autonomous?: boolean;
  manual?: boolean;
  obstacle?: boolean;
  system?: boolean;
};

export type RobotMap = {
  width: number;
  height: number;
  cells: number[][];
  scaleCm?: number; // cm por celda cuando el mapa viene recortado/achicado
};

export type Song = {
  id: number | string;
  name: string;
};

type ApiResult<T> = {
  status?: string;
  data?: T;
  token?: string;
  message?: string;
};

type ServerMap = {
  width: number;
  height: number;
  grid?: number[][];
  cells?: number[][];
  cell_cm?: number;
  robot?: { row: number; col: number; theta_deg?: number };
};

const MAP_MAX_CELLS = 24; // maximo de celdas por lado que se dibujan
const MAP_MIN_CELLS = 8; // minimo, para que un mapa recien iniciado no se vea enorme

/*
 * El servidor entrega la grilla completa (80 x 80) con el robot aparte.
 * La app dibuja una vista por celda y usa 3 para el robot, asi que aqui se
 * recorta la zona explorada, se agrupan celdas si sigue siendo grande
 * (obstaculo > visitada > desconocida) y se marca el robot con 3.
 */
function toRobotMap(server: ServerMap): RobotMap {
  const grid = server.grid ?? server.cells ?? [];
  const robot = server.robot;

  // Formato anterior (sin robot): se usa tal cual.
  if (!robot || grid.length === 0) {
    return { width: server.width, height: server.height, cells: grid };
  }

  const rows = grid.length;
  const cols = grid[0].length;

  let rMin = robot.row;
  let rMax = robot.row;
  let cMin = robot.col;
  let cMax = robot.col;
  for (let r = 0; r < rows; r++) {
    for (let c = 0; c < cols; c++) {
      if (grid[r][c] !== 0) {
        if (r < rMin) rMin = r;
        if (r > rMax) rMax = r;
        if (c < cMin) cMin = c;
        if (c > cMax) cMax = c;
      }
    }
  }

  const span = Math.max(rMax - rMin + 1, cMax - cMin + 1) + 2;
  const factor = Math.max(1, Math.ceil(span / MAP_MAX_CELLS));

  // Los bloques se alinean a la grilla completa, asi no "saltan" cuando el mapa crece.
  const lastRowBlock = Math.floor((rows - 1) / factor);
  const lastColBlock = Math.floor((cols - 1) / factor);
  let br0 = Math.floor(Math.max(0, rMin - 1) / factor);
  let br1 = Math.floor(Math.min(rows - 1, rMax + 1) / factor);
  let bc0 = Math.floor(Math.max(0, cMin - 1) / factor);
  let bc1 = Math.floor(Math.min(cols - 1, cMax + 1) / factor);

  while (br1 - br0 + 1 < MAP_MIN_CELLS && (br0 > 0 || br1 < lastRowBlock)) {
    if (br0 > 0) br0--;
    if (br1 - br0 + 1 < MAP_MIN_CELLS && br1 < lastRowBlock) br1++;
  }
  while (bc1 - bc0 + 1 < MAP_MIN_CELLS && (bc0 > 0 || bc1 < lastColBlock)) {
    if (bc0 > 0) bc0--;
    if (bc1 - bc0 + 1 < MAP_MIN_CELLS && bc1 < lastColBlock) bc1++;
  }

  const robotRow = Math.floor(robot.row / factor);
  const robotCol = Math.floor(robot.col / factor);
  const cells: number[][] = [];

  for (let br = br0; br <= br1; br++) {
    const line: number[] = [];
    for (let bc = bc0; bc <= bc1; bc++) {
      let value = 0;
      for (let r = br * factor; r < (br + 1) * factor && r < rows; r++) {
        for (let c = bc * factor; c < (bc + 1) * factor && c < cols; c++) {
          const cell = grid[r][c];
          if (cell === 2) value = 2;
          else if (cell === 1 && value === 0) value = 1;
        }
      }
      if (br === robotRow && bc === robotCol) value = 3;
      line.push(value);
    }
    cells.push(line);
  }

  return {
    width: bc1 - bc0 + 1,
    height: br1 - br0 + 1,
    cells,
    scaleCm: server.cell_cm ? Math.round(server.cell_cm * factor * 10) / 10 : undefined,
  };
}

type ServerSensors = {
  front_obstacle?: boolean | number;
  side_obstacle?: boolean | number;
};

export function createRobotApi(serverUrl: string, token?: string) {
  const baseUrl = serverUrl.trim().replace(/\/+$/, "");

  async function request<T>(
    endpoint: string,
    method: "GET" | "POST" = "GET",
    values?: Record<string, string | number>,
  ): Promise<ApiResult<T>> {
    const headers: Record<string, string> = { Accept: "application/json" };
    if (values) headers["Content-Type"] = "application/json";
    if (token) headers.Authorization = token;

    const response = await fetch(`${baseUrl}/cgi-bin/${endpoint}`, {
      method,
      headers,
      body: values ? JSON.stringify(values) : undefined,
    });

    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    return (await response.json()) as ApiResult<T>;
  }

  return {
    login: (username: string, password: string) =>
      request<never>("login", "POST", { username, password }),
    status: () => request<RobotStatus>("status"),
    getMode: () => request<{ mode: RobotMode }>("mode"),

    async sensors() {
      const result = await request<ServerSensors>("sensors");
      return {
        ...result,
        data: result.data
          ? {
              front: result.data.front_obstacle,
              side: result.data.side_obstacle,
            }
          : undefined,
      } as ApiResult<SensorData>;
    },
    leds: () => request<LedData>("leds"),
    async map() {
      try {
        const result = await request<ServerMap>("map");
        if (!result.data) return result as ApiResult<RobotMap>;
        return { ...result, data: toRobotMap(result.data) } as ApiResult<RobotMap>;
      } catch (error) {
        // El mapa no es critico: si la navegacion no corre (HTTP 503) no debe
        // tumbar el resto del panel, que se refresca con Promise.all.
        return {
          status: "error",
          message: error instanceof Error ? error.message : "Mapa no disponible",
        } as ApiResult<RobotMap>;
      }
    },
    songs: () => request<{ songs: Song[] }>("audiolist"),
    setMode: (mode: RobotMode) => request<never>("mode", "POST", { mode }),
    move: (direction: string, speed = 65) =>
      request<never>("motors", "POST", { direction, speed }),
    play: (song: Song) => request<never>("audioplay", "POST", { song: song.name }),
    pause: () => request<never>("audiopause", "POST", {}),
    stopAudio: () => request<never>("audiostop", "POST", {}),
    setVolume: (volume: number) => request<never>("audiovolume", "POST", { volume }),
  };
}