let start = Date.now();
const size = 50000000; //50,000,000 Bytes or ~50MB


const a = Array(size);
for(let i = 0; i < size; i++) {
    a[i] = 'a';
}

const b = Array(size);
for(let i = 0; i < size; i++) {
    b[i] = a[i];
}

let end = Date.now();
console.log("\nJS duration in-order access is: " + (end - start) + " milliseconds");


const shuffled = createdShuffledList(size)
let start2 = Date.now();
const a2 = Array(size);
for(let i = 0; i < size; i++) {
    a2[shuffled[i]] = 'a';
}

const b2 = Array(size);
for(let i = 0; i < size; i++) {
    let j = shuffled[i];
    b2[j] = a2[j];
}
let end2 = Date.now();
console.log("JS duration out-of-order access is: " + (end2 - start2) + " milliseconds");
console.log("runtime ratio: " + (end2 - start2)/(end - start));






function createdShuffledList(size) {
    const randomInts = []
    for(let i = 0; i < size; i++) {
        randomInts.push(Math.floor((Math.random()*size)));
    }
    return randomInts;
}



