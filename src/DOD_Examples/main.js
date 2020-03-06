let start = Date.now();
const size = 500000;

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
    a[shuffled[i]] = 'a';
}

const b2 = Array(size);
for(let i = 0; i < size; i++) {
    b2[shuffled[i]] = a[shuffled[i]];
}
let end2 = Date.now();
console.log("JS duration out-of-order access is: " + (end2 - start2) + " milliseconds");






function createdShuffledList(size) {
    const inOrder = []
    for(let i = 0; i < size; i++) {
        inOrder.push(Math.floor(Math.random() * size));
    }
    return inOrder;
}



