const smemory = require('./index');
console.log('Init Share Memory=>', smemory);


let ret = smemory.CreateShareMemory('my-addon-test-memory', 64);
console.log('CreateShareMemory=>', ret);

let writebuff = Buffer.alloc(10);
writebuff[0] = 9;
writebuff[1] = 7;
writebuff[2] = 5;
writebuff[3] = 3;
writebuff[4] = 1;

ret = smemory.WriteShareMemory('my-addon-test-memory', writebuff);
console.log('WriteShareMemory =>', ret);

let readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemory('my-addon-test-memory', readbuff);
console.log('ReadShareMemory => ', ret, readbuff);

writebuff[0] = 1;
writebuff[1] = 3;
writebuff[2] = 5;
writebuff[3] = 7;
writebuff[4] = 9;
ret = smemory.WriteShareMemoryFast('my-addon-test-memory', writebuff);
console.log('WriteShareMemoryFast =>', ret);

readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemoryFast('my-addon-test-memory', readbuff);
console.log('ReadShareMemoryFast => ', ret, readbuff);


ret = smemory.DeleteShareMemory('my-addon-test-memory');
console.log('DeleteShareMemory => ', ret);

readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemory('my-addon-test-memory', readbuff);
console.log('ReadShareMemory => ', ret, readbuff);

smemory.DeleteShareMemory('my-addon-test-memory');
