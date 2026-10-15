async function checkServer()
{
    const connection = document.getElementById("connection");
    const status = document.getElementById("status");

    try {
        const response = await fetch("/cgi-bin/status");

        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }

        const data = await response.json();

        connection.textContent = "Conectado a la Raspberry Pi";
        status.textContent = JSON.stringify(data, null, 2);
    }
    catch (error) {
        connection.textContent = "No se pudo conectar con el servidor";
        status.textContent = error.toString();
    }
}

checkServer();