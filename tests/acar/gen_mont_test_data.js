const crypto = require('crypto');

// Number of sets to generate
const N = 1024;

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

// Main function to generate and display the data
function main(seed) {
    const prng = createSHA256PRNG(BigInt(seed));

    console.log(`
// Generated using tests/gen_mont_test_data.js
char** get_mont_test_data() {
    static char* hex_strs[] = {
`.trim());

    for (let i = 0; i < N; i++) {
        const a = randomBigIntModP(prng);
        const b = randomBigIntModP(prng);
        const ar = (a * r) % p;
        const br = (b * r) % p;
        const abr = (a * br) % p;

        const ar_hex = to64ByteHexString(ar);
        const br_hex = to64ByteHexString(br);
        const abr_hex = to64ByteHexString(abr);

        console.log(`        "${ar_hex}",`);
        console.log(`        "${br_hex}",`);

        if (i < N - 1) {
            console.log(`        "${abr_hex}",`);
        } else {
            console.log(`        "${abr_hex}"`);
        }
    }
    console.log(`
    };
    return hex_strs;
}`);
}

// Only run the main function if this script is executed directly
if (require.main === module) {
    // Get the seed from the command line arguments or use a default seed if not provided
    const seed = process.argv[2] || '1';
    main(seed);
}

