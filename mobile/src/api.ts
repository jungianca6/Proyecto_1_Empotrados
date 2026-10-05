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
};

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
      const result = await request<ServerMap>("map");
      if (!result.data) return result as ApiResult<RobotMap>;
      return {
        ...result,
        data: {
          width: result.data.width,
          height: result.data.height,
          cells: result.data.grid ?? result.data.cells ?? [],
        },
      } as ApiResult<RobotMap>;
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