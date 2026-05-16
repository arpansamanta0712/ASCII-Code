<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ASCII ⇄ Binary Converter</title>
    <style>
        :root {
            --bg-color: #0f172a;
            --card-bg: #1e293b;
            --text-main: #f8fafc;
            --text-muted: #94a3b8;
            --accent: #38bdf8;
            --border: #334155;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: var(--bg-color);
            color: var(--text-main);
            margin: 0;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }

        .container {
            background-color: var(--card-bg);
            padding: 30px;
            border-radius: 12px;
            box-shadow: 0 10px 25px rgba(0,0,0,0.3);
            width: 100%;
            max-width: 600px;
            border: 1px solid var(--border);
        }

        h1 {
            text-align: center;
            margin-top: 0;
            color: var(--accent);
            font-size: 1.8rem;
        }

        p.subtitle {
            text-align: center;
            color: var(--text-muted);
            margin-bottom: 25px;
            font-size: 0.9rem;
        }

        .input-group {
            margin-bottom: 20px;
        }

        label {
            display: block;
            margin-bottom: 8px;
            font-weight: 600;
            color: var(--text-muted);
            font-size: 0.85rem;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }

        textarea {
            width: 100%;
            height: 120px;
            padding: 12px;
            background-color: var(--bg-color);
            border: 1px solid var(--border);
            border-radius: 6px;
            color: var(--text-main);
            font-family: 'Courier New', Courier, monospace;
            font-size: 1rem;
            resize: vertical;
            box-sizing: border-box;
            transition: border-color 0.2s;
        }

        textarea:focus {
            outline: none;
            border-color: var(--accent);
        }

        .error {
            color: #ef4444;
            font-size: 0.85rem;
            margin-top: 5px;
            height: 18px;
        }
    </style>
</head>
<body>

<div class="container">
    <h1>ASCII ⇄ Binary Converter</h1>
    <p class="subtitle">Type text/numbers on the top or binary on the bottom to convert instantly.</p>

    <div class="input-group">
        <label for="textInput">Text / Numbers (ASCII)</label>
        <textarea id="textInput" placeholder="Type letters, numbers, or symbols here..."></textarea>
    </div>

    <div class="input-group">
        <label for="binaryInput">Binary (8-bit spaced)</label>
        <textarea id="binaryInput" placeholder="01000001 01000010 01000011..."></textarea>
        <div id="errorMessage" class="error"></div>
    </div>
</div>

<script>
    const textInput = document.getElementById('textInput');
    const binaryInput = document.getElementById('binaryInput');
    const errorMessage = document.getElementById('errorMessage');

    // Convert Text to Binary
    textInput.addEventListener('input', () => {
        const text = textInput.value;
        if (text === '') {
            binaryInput.value = '';
            errorMessage.textContent = '';
            return;
        }

        const binaryResult = [];
        for (let i = 0; i < text.length; i++) {
            // Get ASCII code and convert to 8-bit binary padding zeros if needed
            let bin = text.charCodeAt(i).toString(2);
            bin = bin.padStart(8, '0');
            binaryResult.push(bin);
        }
        
        binaryInput.value = binaryResult.join(' ');
        errorMessage.textContent = ''; // Clear errors when typing valid text
    });

    // Convert Binary to Text
    binaryInput.addEventListener('input', () => {
        let binaryStr = binaryInput.value.trim();
        
        if (binaryStr === '') {
            textInput.value = '';
            errorMessage.textContent = '';
            return;
        }

        // Remove extra spaces and split by spaces to isolate bytes
        // Also supports a continuous string of binary if split by 8 characters
        if (!binaryStr.includes(' ') && binaryStr.length > 8) {
            binaryStr = binaryStr.match(/.{1,8}/g).join(' ');
        }

        const bytes = binaryStr.split(/\s+/);
        let textResult = '';
        let hasError = false;

        for (let i = 0; i < bytes.length; i++) {
            const byte = bytes[i];
            
            // Validation: Must be only 0s and 1s and ideally up to 8 bits
            if (!/^[01]+$/.test(byte)) {
                hasError = true;
                continue;
            }

            const charCode = parseInt(byte, 2);
            textResult += String.fromCharCode(charCode);
        }

        if (hasError) {
            errorMessage.textContent = "Invalid binary format detected. Use only 0s and 1s.";
        } else {
            errorMessage.textContent = "";
            textInput.value = textResult;
        }
    });
</script>

</body>
</html>
