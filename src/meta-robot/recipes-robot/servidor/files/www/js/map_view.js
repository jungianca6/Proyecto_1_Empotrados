/*
 * Dibuja el mapa de recorrido en un <canvas>.
 *
 * Uso (dentro del polling que ya hace app.js):
 *
 *   const r = await fetch('/cgi-bin/map', { headers: { Authorization: token } });
 *   const j = await r.json();
 *   if (j.status === 'ok') drawMap(document.getElementById('map-canvas'), j.data);
 *
 * Leyenda: 0 = desconocida, 1 = visitada, 2 = obstaculo.
 * Solo dibuja la zona explorada (recorta el area vacia) para que el mapa
 * se vea grande aunque la grilla sea de 80 x 80.
 */
function drawMap(canvas, data) {
  const COLORS = { 0: '#e6e6e6', 1: '#8fd694', 2: '#d9534f' };
  const MIN_SPAN = 20;   // celdas minimas visibles por lado
  const PAD = 3;         // celdas de margen alrededor de lo explorado

  const ctx = canvas.getContext('2d');
  const grid = data.grid;
  const h = data.height, w = data.width;

  // Caja que contiene todo lo explorado y al robot.
  let r0 = data.robot.row, r1 = data.robot.row;
  let c0 = data.robot.col, c1 = data.robot.col;
  for (let r = 0; r < h; r++) {
    for (let c = 0; c < w; c++) {
      if (grid[r][c] !== 0) {
        if (r < r0) r0 = r;
        if (r > r1) r1 = r;
        if (c < c0) c0 = c;
        if (c > c1) c1 = c;
      }
    }
  }
  r0 = Math.max(0, r0 - PAD); r1 = Math.min(h - 1, r1 + PAD);
  c0 = Math.max(0, c0 - PAD); c1 = Math.min(w - 1, c1 + PAD);
  while (r1 - r0 + 1 < MIN_SPAN && (r0 > 0 || r1 < h - 1)) {
    if (r0 > 0) r0--;
    if (r1 - r0 + 1 < MIN_SPAN && r1 < h - 1) r1++;
  }
  while (c1 - c0 + 1 < MIN_SPAN && (c0 > 0 || c1 < w - 1)) {
    if (c0 > 0) c0--;
    if (c1 - c0 + 1 < MIN_SPAN && c1 < w - 1) c1++;
  }

  const rows = r1 - r0 + 1, cols = c1 - c0 + 1;
  const px = Math.max(1, Math.floor(Math.min(canvas.width / cols, canvas.height / rows)));

  ctx.clearRect(0, 0, canvas.width, canvas.height);
  for (let r = r0; r <= r1; r++) {
    for (let c = c0; c <= c1; c++) {
      ctx.fillStyle = COLORS[grid[r][c]] || COLORS[0];
      ctx.fillRect((c - c0) * px, (r - r0) * px, px, px);
    }
  }

  // Robot: circulo con una linea que indica hacia donde mira.
  const rx = (data.robot.col - c0 + 0.5) * px;
  const ry = (data.robot.row - r0 + 0.5) * px;
  const th = data.robot.theta_deg * Math.PI / 180;
  const radius = Math.max(3, px * 1.5);
  ctx.fillStyle = '#1f6feb';
  ctx.beginPath();
  ctx.arc(rx, ry, radius, 0, 2 * Math.PI);
  ctx.fill();
  ctx.strokeStyle = '#ffffff';
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(rx, ry);
  ctx.lineTo(rx + Math.cos(th) * radius * 1.6, ry - Math.sin(th) * radius * 1.6);
  ctx.stroke();
}
