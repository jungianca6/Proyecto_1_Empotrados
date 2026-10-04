export type RobotMode = "autonomous" | "manual";

export type RobotStatus = {
  mode?: RobotMode;
  motor?: string;
  server?: string;
};

export type SensorData = {
  front?: boolean | number;
  left?: boolean | number;
  right?: boolean | number;
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

function formBody(values: Record<string, string | number>): string {
  return Object.entries(values)
    .map(([key, value]) => `${encodeURIComponent(key)}=${encodeURIComponent(value)}`)
    .join("&");
}

export function createRobotApi(serverUrl: string, token?: string) {
  const baseUrl = serverUrl.trim().replace(/\/+$/, "");

  async function request<T>(
    endpoint: string,
    method: "GET" | "POST" = "GET",
    values?: Record<string, string | number>,
  ): Promise<ApiResult<T>> {
    const headers: Record<string, string> = { Accept: "application/json" };
    if (values) headers["Content-Type"] = "application/x-www-form-urlencoded";
    if (token) headers.Authorization = `Bearer ${token}`;

    const response = await fetch(`${baseUrl}/cgi-bin/${endpoint}`, {
      method,
      headers,
      body: values ? formBody(values) : undefined,
    });

    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    return (await response.json()) as ApiResult<T>;
  }

  return {
    login: (username: string, password: string) =>
      request<never>("login", "POST", { username, password }),
    status: () => request<RobotStatus>("status"),
    sensors: () => request<SensorData>("sensors"),
    leds: () => request<LedData>("leds"),
    map: () => request<RobotMap>("map"),
    songs: () => request<{ songs: Song[] }>("audiolist"),
    setMode: (mode: RobotMode) => request<never>("mode", "POST", { mode }),
    move: (command: string) => request<never>("motors", "POST", { command }),
    play: (song: Song) => request<never>("audioplay", "POST", { song: song.name }),
    pause: () => request<never>("audiopause", "POST", {}),
    stopAudio: () => request<never>("audiostop", "POST", {}),
    setVolume: (volume: number) => request<never>("audiovolume", "POST", { volume }),
  };
}