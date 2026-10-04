Simport { useEffect, useState } from "react";
import {
  ActivityIndicator,
  Pressable,
  SafeAreaView,
  ScrollView,
  StyleSheet,
  Text,
  TextInput,
  View,
} from "react-native";
import Slider from "@react-native-community/slider";
import { StatusBar } from "expo-status-bar";
import {
  createRobotApi,
  LedData,
  RobotMap,
  RobotMode,
  RobotStatus,
  SensorData,
  Song,
} from "./src/api";

const initialMap: RobotMap = {
  width: 5,
  height: 5,
  cells: [
    [0, 0, 0, 0, 0],
    [0, 0, 1, 0, 0],
    [0, 0, 1, 0, 0],
    [0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0],
  ],
};
const initialSongs: Song[] = [
  { id: 1, name: "test.mp3" },
  { id: 2, name: "song2.mp3" },
];

function App() {
  const [serverUrl, setServerUrl] = useState("http://192.168.1.100");
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [token, setToken] = useState<string>();
  const [demoMode, setDemoMode] = useState(false);
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState("");
  const [connected, setConnected] = useState(false);
  const [mode, setMode] = useState<RobotMode>("manual");
  const [status, setStatus] = useState<RobotStatus>({ motor: "stopped" });
  const [sensors, setSensors] = useState<SensorData>({});
  const [leds, setLeds] = useState<LedData>({});
  const [robotMap, setRobotMap] = useState(initialMap);
  const [songs, setSongs] = useState(initialSongs);
  const [selectedSong, setSelectedSong] = useState<Song>(initialSongs[0]);
  const [volume, setVolume] = useState(65);
  const [playback, setPlayback] = useState("Detenido");

  useEffect(() => {
    if (!token || demoMode) return;
    const api = createRobotApi(serverUrl, token);

    async function refresh() {
      try {
        const [statusResult, sensorResult, ledResult, mapResult, songResult] =
          await Promise.all([
            api.status(),
            api.sensors(),
            api.leds(),
            api.map(),
            api.songs(),
          ]);
        if (statusResult.data) {
          setStatus(statusResult.data);
          if (statusResult.data.mode) setMode(statusResult.data.mode);
        }
        if (sensorResult.data) setSensors(sensorResult.data);
        if (ledResult.data) setLeds(ledResult.data);
        if (mapResult.data?.cells) setRobotMap(mapResult.data);
        const availableSongs = songResult.data?.songs;
        if (availableSongs?.length) {
          setSongs(availableSongs);
          setSelectedSong((current) =>
            availableSongs.find((song) => song.id === current.id) ?? availableSongs[0],
          );
        }
        setConnected(true);
      } catch {
        setConnected(false);
      }
    }

    void refresh();
    const interval = setInterval(() => void refresh(), 3000);
    return () => clearInterval(interval);
  }, [demoMode, serverUrl, token]);

  function enterDemo() {
    setDemoMode(true);
    setToken("demo-token");
    setConnected(true);
    setError("");
    setMode("manual");
    setStatus({ mode: "manual", motor: "stopped", server: "running" });
    setSensors({ front: true, left: false, right: false });
    setLeds({ system: true, manual: true, autonomous: false, obstacle: true });
    setRobotMap({
      width: 5,
      height: 5,
      cells: [
        [0, 0, 0, 0, 0],
        [0, 1, 1, 1, 0],
        [0, 1, 3, 2, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0],
      ],
    });
    setSongs([
      { id: 1, name: "Recorrido de prueba.mp3" },
      { id: 2, name: "Alerta de obstáculo.mp3" },
      { id: 3, name: "Inicio del sistema.mp3" },
    ]);
    setSelectedSong({ id: 1, name: "Recorrido de prueba.mp3" });
    setPlayback("Detenido");
    setVolume(65);
  }

  function leaveSession() {
    setDemoMode(false);
    setToken(undefined);
    setConnected(false);
  }

  async function signIn() {
    setBusy(true);
    setError("");
    try {
      const result = await createRobotApi(serverUrl).login(username.trim(), password);
      if (result.status !== "ok" || !result.token) {
        throw new Error(result.message || "Inicio de sesión rechazado.");
      }
      setToken(result.token);
    } catch {
      setError("No se pudo iniciar sesión. Verifica la dirección y la conexión Wi-Fi.");
    } finally {
      setBusy(false);
    }
  }

  async function sendCommand(action: () => Promise<unknown>, after?: () => void) {
    setError("");
    if (demoMode) {
      after?.();
      return;
    }
    try {
      await action();
      after?.();
    } catch {
      setConnected(false);
      setError("No se pudo enviar el comando al robot.");
    }
  }

  function chooseMode(nextMode: RobotMode) {
    void sendCommand(
      () => createRobotApi(serverUrl, token).setMode(nextMode),
      () => setMode(nextMode),
    );
  }

  function move(command: string) {
    void sendCommand(
      () => createRobotApi(serverUrl, token).move(command),
      () => setStatus((current) => ({ ...current, motor: command === "stop" ? "stopped" : command })),
    );
  }

  if (!token) {
    return (
      <SafeAreaView style={styles.safeArea}>
        <StatusBar style="dark" />
        <ScrollView contentContainerStyle={styles.loginPage} keyboardShouldPersistTaps="handled">
          <View style={styles.loginTopline}>
            <View style={styles.brandMark}><Text style={styles.brandMarkText}>R</Text></View>
            <Text style={styles.kicker}>CONTROL DE ROBOT</Text>
          </View>
          <View style={styles.loginHero}>
            <Text style={styles.loginTitle}>Rover<Text style={styles.titleDot}>.</Text></Text>
            <Text style={styles.loginSubtitle}>Control remoto de limpieza</Text>
            <View style={styles.heroRule} />
            <Text style={styles.loginDescription}>Conéctate a tu robot para monitorear su recorrido y tomar el control.</Text>
          </View>
          <View style={styles.loginForm}>
            <Text style={styles.sectionEyebrow}>INICIAR SESIÓN</Text>
            <Text style={styles.inputLabel}>Dirección del servidor</Text>
            <TextInput autoCapitalize="none" autoCorrect={false} keyboardType="url" onChangeText={setServerUrl} placeholder="http://192.168.1.100" placeholderTextColor={colors.muted} style={styles.input} value={serverUrl} />
            <Text style={styles.inputLabel}>Usuario</Text>
            <TextInput autoCapitalize="none" autoCorrect={false} onChangeText={setUsername} placeholder="Nombre de usuario" placeholderTextColor={colors.muted} style={styles.input} value={username} />
            <Text style={styles.inputLabel}>Contraseña</Text>
            <TextInput onChangeText={setPassword} onSubmitEditing={() => void signIn()} placeholder="Contraseña" placeholderTextColor={colors.muted} secureTextEntry style={styles.input} value={password} />
            {error ? <Text style={styles.errorText}>{error}</Text> : null}
            <Pressable disabled={busy} onPress={() => void signIn()} style={({ pressed }) => [styles.primaryButton, pressed && styles.pressed, busy && styles.disabled]}>
              {busy ? <ActivityIndicator color={colors.white} /> : <Text style={styles.primaryButtonText}>Conectar <Text style={styles.buttonArrow}>→</Text></Text>}
            </Pressable>
            <Pressable onPress={enterDemo} style={styles.demoButton}>
              <Text style={styles.demoButtonText}>Probar en modo demo <Text style={styles.demoArrow}>→</Text></Text>
            </Pressable>
          </View>
          <Text style={styles.loginFooter}>ROBOT ASPIRADORA  /  CE-1113</Text>
        </ScrollView>
      </SafeAreaView>
    );
  }

  const isAutonomous = mode === "autonomous";
  const sensorRows = [
    { key: "front" as const, label: "Frontal" },
    { key: "left" as const, label: "Izquierdo" },
    { key: "right" as const, label: "Derecho" },
  ];
  const ledRows = [
    { key: "system" as const, label: "Sistema" },
    { key: "autonomous" as const, label: "Autónomo" },
    { key: "manual" as const, label: "Manual" },
    { key: "obstacle" as const, label: "Obstáculo" },
  ];

  return (
    <SafeAreaView style={styles.safeArea}>
      <StatusBar style="dark" />
      <ScrollView contentContainerStyle={styles.dashboard}>
        <View style={styles.topBar}>
          <View style={styles.brandLockup}>
            <View style={styles.brandMarkSmall}><Text style={styles.brandMarkText}>R</Text></View>
            <View><Text style={styles.brandName}>ROVER</Text><Text style={styles.brandCaption}>PANEL DE CONTROL</Text></View>
          </View>
          <Pressable onPress={leaveSession} style={styles.connectionPill}>
            <View style={[styles.connectionDot, connected ? styles.onlineDot : styles.offlineDot]} />
            <Text style={styles.connectionText}>{demoMode ? "DEMO LOCAL" : connected ? "EN LÍNEA" : "SIN SEÑAL"}</Text>
          </Pressable>
        </View>

        <View style={styles.greetingRow}>
          <View><Text style={styles.pageEyebrow}>ROBOT ASPIRADORA</Text><Text style={styles.pageTitle}>Panel de control</Text></View>
          <View style={styles.liveBadge}><View style={styles.liveDot} /><Text style={styles.liveText}>LIVE</Text></View>
        </View>

        <View style={styles.modeSection}>
          <View style={styles.sectionHeadingRow}><Text style={styles.sectionTitle}>Modo de operación</Text><Text style={styles.motorState}>{status.motor === "stopped" ? "MOTORES EN ESPERA" : "MOTORES ACTIVOS"}</Text></View>
          <View style={styles.segmented}>
            <Pressable onPress={() => chooseMode("manual")} style={[styles.segment, !isAutonomous && styles.segmentActive]}><Text style={[styles.segmentIcon, !isAutonomous && styles.segmentTextActive]}>⌘</Text><Text style={[styles.segmentText, !isAutonomous && styles.segmentTextActive]}>Manual</Text></Pressable>
            <Pressable onPress={() => chooseMode("autonomous")} style={[styles.segment, isAutonomous && styles.segmentActive]}><Text style={[styles.segmentIcon, isAutonomous && styles.segmentTextActive]}>◎</Text><Text style={[styles.segmentText, isAutonomous && styles.segmentTextActive]}>Autónomo</Text></Pressable>
          </View>
        </View>

        <View style={styles.sectionHeadingRow}><Text style={styles.sectionTitle}>Movimiento</Text><Text style={styles.sectionMeta}>{isAutonomous ? "CONTROL AUTOMÁTICO" : "CONTROL MANUAL"}</Text></View>
        <View style={styles.motionPanel}>
          <View style={styles.dpad}>
            <View style={styles.dpadRow}><View style={styles.dpadBlank} /><DirectionButton label="↑" command="forward" onPress={move} disabled={isAutonomous} /><View style={styles.dpadBlank} /></View>
            <View style={styles.dpadRow}><DirectionButton label="←" command="left" onPress={move} disabled={isAutonomous} /><Pressable onPress={() => move("stop")} disabled={isAutonomous} style={[styles.stopButton, isAutonomous && styles.disabled]}><View style={styles.stopSquare} /></Pressable><DirectionButton label="→" command="right" onPress={move} disabled={isAutonomous} /></View>
            <View style={styles.dpadRow}><View style={styles.dpadBlank} /><DirectionButton label="↓" command="backward" onPress={move} disabled={isAutonomous} /><View style={styles.dpadBlank} /></View>
          </View>
          <View style={styles.motionDivider} />
          <View style={styles.motionAside}><Text style={styles.motionAsideLabel}>MODO ACTIVO</Text><Text style={styles.motionAsideValue}>{isAutonomous ? "Autónomo" : "Manual"}</Text><Text style={styles.motionAsideHint}>{isAutonomous ? "El robot navega por su cuenta." : "Usa las flechas para conducir."}</Text></View>
        </View>

        <View style={styles.sectionHeadingRow}><Text style={styles.sectionTitle}>Sensores</Text><Text style={styles.sectionMeta}>PROXIMIDAD</Text></View>
        <View style={styles.sensorPanel}>
          {sensorRows.map((sensor, index) => {
            const detected = Boolean(sensors[sensor.key]);
            return <View key={sensor.key} style={[styles.sensorItem, index < sensorRows.length - 1 && styles.sensorBorder]}><View style={[styles.sensorIcon, detected && styles.sensorIconAlert]}><View style={[styles.sensorPulse, detected && styles.sensorPulseAlert]} /></View><View style={styles.sensorLabelGroup}><Text style={styles.sensorLabel}>{sensor.label}</Text><Text style={styles.sensorReading}>{detected ? "Obstáculo" : "Despejado"}</Text></View><View style={[styles.sensorIndicator, detected ? styles.sensorAlert : styles.sensorClear]} /><Text style={[styles.sensorState, detected && styles.sensorStateAlert]}>{detected ? "ALERTA" : "LIBRE"}</Text></View>;
          })}
        </View>

        <View style={styles.sectionHeadingRow}><Text style={styles.sectionTitle}>Mapa de recorrido</Text><Text style={styles.sectionMeta}>GRILLA 2D</Text></View>
        <View style={styles.mapPanel}>
          <View style={styles.mapTopline}><Text style={styles.mapCaption}>ÁREA EXPLORADA</Text><Text style={styles.mapCoordinate}>{robotMap.width} × {robotMap.height}</Text></View>
          <View style={styles.mapGrid}>
            {Array.from({ length: robotMap.height }, (_, row) => (
              <View key={`row-${row}`} style={styles.mapRow}>
                {Array.from({ length: robotMap.width }, (_, column) => {
                  const value = robotMap.cells[row]?.[column] ?? 0;
                  return <View key={`${row}-${column}`} style={[styles.mapCell, value === 1 && styles.mapVisited, value === 2 && styles.mapObstacle, value === 3 && styles.mapRobot]}>{value === 3 ? <View style={styles.mapRobotCore} /> : null}</View>;
                })}
              </View>
            ))}
          </View>
          <View style={styles.legend}><LegendItem color={colors.cellVisited} label="Visitada" /><LegendItem color={colors.cellObstacle} label="Obstáculo" /><LegendItem color={colors.cellUnknown} label="Desconocida" /></View>
        </View>

        <View style={styles.sectionHeadingRow}><Text style={styles.sectionTitle}>Reproductor</Text><Text style={styles.sectionMeta}>AUDIO MP3</Text></View>
        <View style={styles.audioPanel}>
          <Text style={styles.audioEyebrow}>LISTA DE REPRODUCCIÓN</Text>
          {songs.map((song, index) => (
            <Pressable key={song.id} onPress={() => setSelectedSong(song)} style={[styles.songRow, index < songs.length - 1 && styles.songBorder]}>
              <View style={[styles.songIndex, selectedSong.id === song.id && styles.songIndexActive]}><Text style={[styles.songIndexText, selectedSong.id === song.id && styles.songIndexTextActive]}>{String(index + 1).padStart(2, "0")}</Text></View>
              <Text numberOfLines={1} style={styles.songName}>{song.name}</Text><Text style={styles.songSelected}>{selectedSong.id === song.id ? "●" : ""}</Text>
            </Pressable>
          ))}
          <View style={styles.audioNowPlaying}><View style={styles.audioDisc}><Text style={styles.audioNote}>♫</Text></View><View style={styles.audioTrack}><Text style={styles.audioTrackLabel}>SELECCIONADA</Text><Text numberOfLines={1} style={styles.audioTrackName}>{selectedSong.name}</Text></View><Text style={styles.playbackState}>{playback.toUpperCase()}</Text></View>
          <View style={styles.audioControls}>
            <Pressable onPress={() => { setPlayback("Reproduciendo"); void sendCommand(() => createRobotApi(serverUrl, token).play(selectedSong)); }} style={styles.playButton}><Text style={styles.playIcon}>▶</Text></Pressable>
            <Pressable onPress={() => { setPlayback("En pausa"); void sendCommand(() => createRobotApi(serverUrl, token).pause()); }} style={styles.audioControlButton}><Text style={styles.controlGlyph}>Ⅱ</Text></Pressable>
            <Pressable onPress={() => { setPlayback("Detenido"); void sendCommand(() => createRobotApi(serverUrl, token).stopAudio()); }} style={styles.audioControlButton}><View style={styles.stopGlyph} /></Pressable>
          </View>
          <View style={styles.volumeHeader}><Text style={styles.volumeLabel}>VOLUMEN</Text><Text style={styles.volumeValue}>{Math.round(volume)}%</Text></View>
          <Slider minimumValue={0} maximumValue={100} step={1} value={volume} minimumTrackTintColor={colors.ink} maximumTrackTintColor={colors.line} thumbTintColor={colors.coral} onValueChange={setVolume} onSlidingComplete={(value) => void sendCommand(() => createRobotApi(serverUrl, token).setVolume(value))} />
        </View>

        <View style={styles.sectionHeadingRow}><Text style={styles.sectionTitle}>Indicadores</Text><Text style={styles.sectionMeta}>LEDS FÍSICOS</Text></View>
        <View style={styles.ledPanel}>
          {ledRows.map((led) => <View key={led.key} style={styles.ledItem}><View style={[styles.ledDot, leds[led.key] ? styles.ledOn : styles.ledOff]} /><Text style={styles.ledLabel}>{led.label}</Text><Text style={[styles.ledState, leds[led.key] && styles.ledStateOn]}>{leds[led.key] ? "ON" : "OFF"}</Text></View>)}
        </View>
        {error ? <Text style={styles.errorText}>{error}</Text> : null}
        <Pressable onPress={leaveSession} style={styles.logoutButton}><Text style={styles.logoutText}>Cerrar sesión</Text><Text style={styles.logoutArrow}>↗</Text></Pressable>
        <Text style={styles.footerText}>ROVER  /  SISTEMA DE CONTROL EMBEBIDO</Text>
      </ScrollView>
    </SafeAreaView>
  );
}

function DirectionButton({ label, command, disabled, onPress }: { label: string; command: string; disabled: boolean; onPress: (command: string) => void }) {
  return <Pressable disabled={disabled} onPress={() => onPress(command)} style={({ pressed }) => [styles.directionButton, pressed && styles.directionPressed, disabled && styles.disabled]}><Text style={styles.directionText}>{label}</Text></Pressable>;
}

function LegendItem({ color, label }: { color: string; label: string }) {
  return <View style={styles.legendItem}><View style={[styles.legendSwatch, { backgroundColor: color }]} /><Text style={styles.legendLabel}>{label}</Text></View>;
}

const colors = {
  ink: "#202820", muted: "#839087", white: "#FFFFFF", paper: "#F4F6F1",
  panel: "#FFFFFF", line: "#E4E9E2", green: "#417A55", greenSoft: "#E9F2E9",
  coral: "#D86E50", coralSoft: "#F9EBE5", cellUnknown: "#EFF2ED",
  cellVisited: "#A8C9A9", cellObstacle: "#E98D70",
};

const styles = StyleSheet.create({
  safeArea: { flex: 1, backgroundColor: colors.paper },
  loginPage: { flexGrow: 1, paddingHorizontal: 26, paddingTop: 26, paddingBottom: 28 },
  loginTopline: { flexDirection: "row", alignItems: "center", gap: 11 },
  brandMark: { width: 38, height: 38, borderRadius: 12, backgroundColor: colors.green, alignItems: "center", justifyContent: "center" },
  brandMarkText: { color: colors.white, fontSize: 20, fontWeight: "800" },
  kicker: { color: colors.green, fontSize: 11, fontWeight: "800", letterSpacing: 1.2 },
  loginHero: { paddingTop: 46, paddingBottom: 34 },
  loginTitle: { color: colors.ink, fontSize: 54, lineHeight: 60, fontWeight: "800", letterSpacing: -1.5 },
  titleDot: { color: colors.coral },
  loginSubtitle: { color: colors.green, fontSize: 17, fontWeight: "700", marginTop: 6 },
  heroRule: { width: 46, height: 3, backgroundColor: colors.coral, marginTop: 23, marginBottom: 17 },
  loginDescription: { color: "#667169", fontSize: 15, lineHeight: 23, maxWidth: 310 },
  loginForm: { backgroundColor: colors.panel, padding: 21, borderRadius: 8, borderWidth: 1, borderColor: colors.line },
  sectionEyebrow: { color: colors.green, fontSize: 10, fontWeight: "800", letterSpacing: 1.2, marginBottom: 20 },
  inputLabel: { color: colors.ink, fontSize: 12, fontWeight: "700", marginBottom: 7, marginTop: 12 },
  input: { minHeight: 48, borderWidth: 1, borderColor: colors.line, borderRadius: 5, paddingHorizontal: 13, color: colors.ink, fontSize: 14, backgroundColor: "#FBFCFA" },
  primaryButton: { height: 52, backgroundColor: colors.green, borderRadius: 5, alignItems: "center", justifyContent: "center", marginTop: 21, flexDirection: "row" },
  primaryButtonText: { color: colors.white, fontSize: 15, fontWeight: "800" },
  buttonArrow: { color: "#DDEBDD", fontSize: 19 }, pressed: { opacity: 0.83 }, disabled: { opacity: 0.38 },
  demoButton: { height: 46, alignItems: "center", justifyContent: "center", marginTop: 5 },
  demoButtonText: { color: colors.green, fontSize: 13, fontWeight: "800" }, demoArrow: { color: colors.coral, fontSize: 16 },
  loginFooter: { marginTop: "auto", paddingTop: 28, color: colors.muted, fontSize: 9, fontWeight: "700", letterSpacing: 1.1 },
  errorText: { color: "#AE4934", fontSize: 12, lineHeight: 18, marginTop: 12 },
  dashboard: { paddingHorizontal: 19, paddingBottom: 34 },
  topBar: { height: 67, borderBottomWidth: 1, borderBottomColor: colors.line, flexDirection: "row", alignItems: "center", justifyContent: "space-between" },
  brandLockup: { flexDirection: "row", alignItems: "center", gap: 9 },
  brandMarkSmall: { width: 31, height: 31, borderRadius: 9, backgroundColor: colors.green, alignItems: "center", justifyContent: "center" },
  brandName: { color: colors.ink, fontSize: 12, fontWeight: "900", letterSpacing: 1.3 },
  brandCaption: { color: colors.muted, fontSize: 8, fontWeight: "700", letterSpacing: 0.7, marginTop: 2 },
  connectionPill: { minHeight: 30, borderRadius: 15, paddingHorizontal: 10, backgroundColor: colors.white, borderWidth: 1, borderColor: colors.line, flexDirection: "row", alignItems: "center", gap: 6 },
  connectionDot: { width: 7, height: 7, borderRadius: 4 }, onlineDot: { backgroundColor: colors.green }, offlineDot: { backgroundColor: colors.coral },
  connectionText: { color: colors.ink, fontSize: 9, fontWeight: "800", letterSpacing: 0.4 },
  greetingRow: { paddingTop: 22, paddingBottom: 21, flexDirection: "row", justifyContent: "space-between", alignItems: "flex-end" },
  pageEyebrow: { color: colors.green, fontSize: 9, fontWeight: "800", letterSpacing: 1.1, marginBottom: 5 },
  pageTitle: { color: colors.ink, fontSize: 24, lineHeight: 29, fontWeight: "800" },
  liveBadge: { flexDirection: "row", alignItems: "center", gap: 5, paddingBottom: 3 }, liveDot: { width: 6, height: 6, borderRadius: 4, backgroundColor: colors.coral },
  liveText: { color: colors.coral, fontSize: 9, fontWeight: "900", letterSpacing: 0.8 }, modeSection: { marginBottom: 21 },
  sectionHeadingRow: { flexDirection: "row", justifyContent: "space-between", alignItems: "center", marginBottom: 10 },
  sectionTitle: { color: colors.ink, fontSize: 15, fontWeight: "800" },
  sectionMeta: { color: colors.muted, fontSize: 8, fontWeight: "800", letterSpacing: 0.8 },
  motorState: { color: colors.muted, fontSize: 8, fontWeight: "800", letterSpacing: 0.6 },
  segmented: { flexDirection: "row", backgroundColor: "#E9EDE7", padding: 4, borderRadius: 6 },
  segment: { flex: 1, height: 43, borderRadius: 4, alignItems: "center", justifyContent: "center", flexDirection: "row", gap: 8 }, segmentActive: { backgroundColor: colors.ink },
  segmentIcon: { color: colors.muted, fontSize: 17, fontWeight: "700" }, segmentText: { color: "#647066", fontSize: 12, fontWeight: "700" }, segmentTextActive: { color: colors.white },
  motionPanel: { flexDirection: "row", alignItems: "center", backgroundColor: colors.panel, borderRadius: 8, borderWidth: 1, borderColor: colors.line, paddingVertical: 15, paddingHorizontal: 10, marginBottom: 21 },
  dpad: { width: 174, gap: 5 }, dpadRow: { flexDirection: "row", justifyContent: "center", gap: 5 }, dpadBlank: { width: 47, height: 43 },
  directionButton: { width: 47, height: 43, borderRadius: 5, backgroundColor: colors.greenSoft, alignItems: "center", justifyContent: "center" },
  directionPressed: { backgroundColor: "#D4E6D5" }, directionText: { color: colors.green, fontSize: 23, fontWeight: "700", lineHeight: 27 },
  stopButton: { width: 47, height: 43, borderRadius: 5, backgroundColor: colors.coralSoft, alignItems: "center", justifyContent: "center" }, stopSquare: { width: 12, height: 12, borderRadius: 2, backgroundColor: colors.coral },
  motionDivider: { width: 1, alignSelf: "stretch", backgroundColor: colors.line, marginHorizontal: 13 }, motionAside: { flex: 1, minWidth: 82 },
  motionAsideLabel: { color: colors.muted, fontSize: 8, fontWeight: "800", letterSpacing: 0.7 }, motionAsideValue: { color: colors.ink, fontSize: 14, fontWeight: "800", marginTop: 6 },
  motionAsideHint: { color: colors.muted, fontSize: 10, lineHeight: 15, marginTop: 4 },
  sensorPanel: { backgroundColor: colors.panel, borderRadius: 8, borderWidth: 1, borderColor: colors.line, paddingHorizontal: 13, marginBottom: 21 },
  sensorItem: { minHeight: 58, flexDirection: "row", alignItems: "center", gap: 10 }, sensorBorder: { borderBottomWidth: 1, borderBottomColor: colors.line },
  sensorIcon: { width: 28, height: 28, borderRadius: 14, backgroundColor: colors.greenSoft, alignItems: "center", justifyContent: "center" }, sensorIconAlert: { backgroundColor: colors.coralSoft },
  sensorPulse: { width: 9, height: 9, borderRadius: 5, backgroundColor: colors.green }, sensorPulseAlert: { backgroundColor: colors.coral }, sensorLabelGroup: { flex: 1 },
  sensorLabel: { color: colors.ink, fontSize: 12, fontWeight: "700" }, sensorReading: { color: colors.muted, fontSize: 10, marginTop: 3 },
  sensorIndicator: { width: 6, height: 6, borderRadius: 4 }, sensorClear: { backgroundColor: colors.green }, sensorAlert: { backgroundColor: colors.coral },
  sensorState: { width: 45, textAlign: "right", color: colors.green, fontSize: 8, fontWeight: "900", letterSpacing: 0.6 }, sensorStateAlert: { color: colors.coral },
  mapPanel: { backgroundColor: colors.panel, borderRadius: 8, borderWidth: 1, borderColor: colors.line, padding: 15, marginBottom: 21 },
  mapTopline: { flexDirection: "row", justifyContent: "space-between", marginBottom: 14 }, mapCaption: { color: colors.muted, fontSize: 8, fontWeight: "800", letterSpacing: 0.8 },
  mapCoordinate: { color: colors.green, fontSize: 9, fontWeight: "800" }, mapGrid: { alignSelf: "center", gap: 4 }, mapRow: { flexDirection: "row", gap: 4 },
  mapCell: { width: 32, aspectRatio: 1, borderRadius: 4, backgroundColor: colors.cellUnknown, borderWidth: 1, borderColor: "#E6EBE4", alignItems: "center", justifyContent: "center" },
  mapVisited: { backgroundColor: colors.cellVisited, borderColor: "#99BC9A" }, mapObstacle: { backgroundColor: colors.cellObstacle, borderColor: "#DA795D" }, mapRobot: { backgroundColor: colors.green, borderColor: colors.green },
  mapRobotCore: { width: 9, height: 9, borderRadius: 5, backgroundColor: colors.white }, legend: { flexDirection: "row", justifyContent: "center", gap: 15, marginTop: 15, flexWrap: "wrap" },
  legendItem: { flexDirection: "row", alignItems: "center", gap: 5 }, legendSwatch: { width: 9, height: 9, borderRadius: 2 }, legendLabel: { color: colors.muted, fontSize: 9 },
  audioPanel: { backgroundColor: colors.panel, borderRadius: 8, borderWidth: 1, borderColor: colors.line, padding: 14, marginBottom: 21 },
  audioEyebrow: { color: colors.muted, fontSize: 8, fontWeight: "800", letterSpacing: 0.8, marginBottom: 5 }, songRow: { height: 39, flexDirection: "row", alignItems: "center", gap: 10 }, songBorder: { borderBottomWidth: 1, borderBottomColor: colors.line },
  songIndex: { width: 26, height: 26, borderRadius: 4, alignItems: "center", justifyContent: "center", backgroundColor: colors.paper }, songIndexActive: { backgroundColor: colors.greenSoft }, songIndexText: { color: colors.muted, fontSize: 9, fontWeight: "800" }, songIndexTextActive: { color: colors.green },
  songName: { flex: 1, color: colors.ink, fontSize: 11, fontWeight: "700" }, songSelected: { color: colors.green, width: 14, textAlign: "center", fontSize: 10 },
  audioNowPlaying: { flexDirection: "row", alignItems: "center", backgroundColor: colors.paper, borderRadius: 5, padding: 9, marginTop: 13 }, audioDisc: { width: 34, height: 34, borderRadius: 17, backgroundColor: colors.ink, alignItems: "center", justifyContent: "center" },
  audioNote: { color: colors.white, fontSize: 16 }, audioTrack: { flex: 1, marginLeft: 9 }, audioTrackLabel: { color: colors.muted, fontSize: 7, fontWeight: "800", letterSpacing: 0.7 }, audioTrackName: { color: colors.ink, fontSize: 10, fontWeight: "700", marginTop: 3 }, playbackState: { color: colors.green, fontSize: 7, fontWeight: "900", letterSpacing: 0.5 },
  audioControls: { flexDirection: "row", justifyContent: "center", alignItems: "center", gap: 12, marginTop: 13, marginBottom: 14 }, playButton: { width: 38, height: 38, borderRadius: 19, backgroundColor: colors.green, alignItems: "center", justifyContent: "center" }, playIcon: { color: colors.white, fontSize: 13, marginLeft: 2 },
  audioControlButton: { width: 34, height: 34, borderRadius: 17, backgroundColor: colors.paper, alignItems: "center", justifyContent: "center" }, controlGlyph: { color: colors.ink, fontSize: 15, fontWeight: "800" }, stopGlyph: { width: 10, height: 10, borderRadius: 1, backgroundColor: colors.ink },
  volumeHeader: { flexDirection: "row", justifyContent: "space-between", marginTop: 3 }, volumeLabel: { color: colors.muted, fontSize: 8, fontWeight: "800", letterSpacing: 0.7 }, volumeValue: { color: colors.ink, fontSize: 9, fontWeight: "800" },
  ledPanel: { flexDirection: "row", flexWrap: "wrap", backgroundColor: colors.panel, borderRadius: 8, borderWidth: 1, borderColor: colors.line, paddingVertical: 5, marginBottom: 17 }, ledItem: { width: "50%", height: 40, flexDirection: "row", alignItems: "center", paddingHorizontal: 12, gap: 7 },
  ledDot: { width: 8, height: 8, borderRadius: 5 }, ledOn: { backgroundColor: colors.green }, ledOff: { backgroundColor: "#D3D9D2" }, ledLabel: { flex: 1, color: colors.ink, fontSize: 10, fontWeight: "600" }, ledState: { color: colors.muted, fontSize: 8, fontWeight: "800" }, ledStateOn: { color: colors.green },
  logoutButton: { height: 43, flexDirection: "row", justifyContent: "center", alignItems: "center", gap: 7, borderWidth: 1, borderColor: colors.line, borderRadius: 5, backgroundColor: colors.white }, logoutText: { color: colors.ink, fontSize: 11, fontWeight: "700" }, logoutArrow: { color: colors.muted, fontSize: 14 },
  footerText: { textAlign: "center", color: colors.muted, fontSize: 8, fontWeight: "700", letterSpacing: 0.9, marginTop: 17 },
});

export default App;