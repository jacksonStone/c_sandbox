var q = [];
for(let i = 0; i < 100000; i++){ q.push(`[
    {"foo${i}":-1234.5678, "gee":-2234.5678, "fee": {"bar":"baz"}, "bop": [123, 456, null]},
    {"foo${i + 1}":-1234.5678, "gee":-2234.5678, "fee": {"baro":"baz"}, "bop": [123, 456, null]},
    {"foo${i + 2}":-1234.5678, "gee":-2234.5678, "fee": {"bare":"baz"}, "bop": [123, 456, null]},
    {"foo${i + 3}":-1234.5678, "gee":-2234.5678, "fee": {"bara":"baz"}, "bop": [123, 456, null]}
]`);}
var a = Date.now();
for(let i = 0; i < 100000; i++){ var p = JSON.parse(q[i]); }

 b = Date.now(); console.log('NODE: ' + ((b - a)/100) + ' microseconds per run');