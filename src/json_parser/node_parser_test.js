var a = Date.now(); var p = []; 
for(let i = 0; i < 100000; i++){ p.push(JSON.parse(`[
    {"foo":-1234.5678, "gee":-2234.5678, "fee": {"bar":"baz"}, "bop": [123, 456, null]},
    {"foo":-1234.5678, "gee":-2234.5678, "fee": {"baro":"baz"}, "bop": [123, 456, null]},
    {"foo":-1234.5678, "gee":-2234.5678, "fee": {"bare":"baz"}, "bop": [123, 456, null]},
    {"foo":-1234.5678, "gee":-2234.5678, "fee": {"bara":"baz"}, "bop": [123, 456, null]}
]`));}

 b = Date.now(); console.log('NODE: ' + ((b - a)/100) + ' microseconds per run')
