const crypto = require('crypto');

const cost = 1024;
const p = BigInt('0x30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001');
const r = BigInt('0x10000000000000000000000000000000000000000000000000000000000000000');

// PRNG function using SHA256 based on a seed
function createSHA256PRNG(seed) {
    let counter = BigInt(seed);

    return function() {
        // Convert counter to a buffer
        const counterBuffer = Buffer.from(counter.toString(16).padStart(64, '0'), 'hex');
        
        // Hash the buffer using SHA256
        const hash = crypto.createHash('sha256').update(counterBuffer).digest();

        // Convert the hash to a BigInt
        const randomBigInt = BigInt('0x' + hash.toString('hex'));

        // Increment the counter for the next call
        counter += 1n;

        return randomBigInt;
    };
}

// Function to generate a random BigInt mod p using the PRNG
function randomBigIntModP(prng) {
    let result = BigInt(0);
    for (let i = 0; i < 4; i++) {
        // Generate 64 bits at a time and combine them
        result = (result << 64n) | (prng() & BigInt('0xFFFFFFFFFFFFFFFF'));
    }
    return result % p;
}

// Function to convert a BigInt to a 64-byte hex string
function to64ByteHexString(bigIntValue) {
    return bigIntValue.toString(16).padStart(64, '0');
}

function reference_func(a, b, cost) {
    let x = a;
    let y = b;
    let z;
    for (let i = 0; i < cost; i ++) {
        z = (x * y) % p;
        x = y;
        y = z;
    }
    return (y * r) % p;
}

// Main function to generate and display the data
function main(seed) {
    const start = 10;
    const end = 21;
    const prng = createSHA256PRNG(BigInt(seed));
    const a = randomBigIntModP(prng);
    const b = randomBigIntModP(prng);

console.log(`typedef struct {
    int   cost;
    char* a_hex;
    char* b_hex;
    char* result_hex;
} BenchmarkData;

const int get_benchmark_data_length() {
    return ${end - start};
}

const BenchmarkData* get_benchmark_data() {
    static const BenchmarkData dataArray[] = {`);

    for (var i = start; i < end; i++) {
        const cost = 2 ** i;
        const result = reference_func(a, b, cost);
        const ar = a * r % p;
        const br = b * r % p;

        let entry = "        {\n";
        entry += `            ${cost},\n`;
        entry += `            "${to64ByteHexString(ar)}",\n`;
        entry += `            "${to64ByteHexString(br)}",\n`;
        entry += `            "${to64ByteHexString(result)}"\n`;

        if (i < end - 1) {
            entry += `        },\n`;
        } else {
            entry += `        }\n`;
        }
        console.log(entry)
    }
    console.log(`    };`);
    console.log(`    return dataArray;`);
    console.log(`}`);
}

// Only run the main function if this script is executed directly
if (require.main === module) {
    // Get the seed from the command line arguments or use a default seed if not provided
    const seed = process.argv[2] || '1';
    main(seed);
}
