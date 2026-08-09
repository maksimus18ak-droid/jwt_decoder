// jwt_decoder.js
const fs = require('fs');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers');
const chalk = require('chalk');

function base64UrlDecode(data) {
    // Добавляем padding
    while (data.length % 4 !== 0) {
        data += '=';
    }
    return Buffer.from(data.replace(/-/g, '+').replace(/_/g, '/'), 'base64').toString('utf8');
}

function colorizeValue(value, indent, level) {
    const spaces = ' '.repeat(level * indent);
    if (typeof value === 'object' && value !== null && !Array.isArray(value)) {
        const keys = Object.keys(value);
        if (keys.length === 0) return '{}';
        const lines = ['{'];
        keys.forEach((key, i) => {
            const valStr = colorizeValue(value[key], indent, level + 1);
            let line = spaces + ' '.repeat(indent) + chalk.blue(`"${key}"`) + ': ' + valStr;
            if (i < keys.length - 1) line += ',';
            lines.push(line);
        });
        lines.push(spaces + '}');
        return lines.join('\n');
    } else if (Array.isArray(value)) {
        if (value.length === 0) return '[]';
        const lines = ['['];
        value.forEach((item, i) => {
            let line = spaces + ' '.repeat(indent) + colorizeValue(item, indent, level + 1);
            if (i < value.length - 1) line += ',';
            lines.push(line);
        });
        lines.push(spaces + ']');
        return lines.join('\n');
    } else if (typeof value === 'string') {
        return chalk.green(`"${value}"`);
    } else if (typeof value === 'boolean') {
        return chalk.magenta(value);
    } else if (value === null) {
        return chalk.gray('null');
    } else if (typeof value === 'number') {
        return chalk.yellow(value);
    } else {
        return String(value);
    }
}

function colorizeJSON(data, indent) {
    return colorizeValue(data, indent, 0);
}

function decodeJwt(token) {
    const parts = token.split('.');
    if (parts.length !== 3) throw new Error('Неверный JWT: ожидается 3 сегмента');
    const [headerB64, payloadB64, sig] = parts;
    const headerStr = base64UrlDecode(headerB64);
    const payloadStr = base64UrlDecode(payloadB64);
    const header = JSON.parse(headerStr);
    const payload = JSON.parse(payloadStr);
    return { header, payload, sig };
}

async function main() {
    const argv = yargs(hideBin(process.argv))
        .usage('Использование: $0 <token> [--file FILE] [--compact]')
        .option('file', { type: 'string', description: 'Читать токен из файла' })
        .option('compact', { type: 'boolean', description: 'Вывод без отступов' })
        .help()
        .parse();

    let token = argv._[0];
    if (argv.file) {
        token = fs.readFileSync(argv.file, 'utf8').trim();
    }
    if (!token) {
        console.error('Не указан JWT-токен.');
        process.exit(1);
    }
    let { header, payload, sig } = decodeJwt(token);
    const indent = argv.compact ? 0 : 2;
    console.log(chalk.cyan('--- Header ---'));
    console.log(colorizeJSON(header, indent));
    console.log(chalk.cyan('--- Payload ---'));
    console.log(colorizeJSON(payload, indent));
    console.log(chalk.gray('--- Signature ---'));
    console.log(chalk.gray(sig));
}

main().catch(console.error);
