# share-memory-mac
a share memory module between diffrent process on windows. runtime nodejs/electron.

## example
```js
const smemory = require('share-memory-mac');
console.log('Init Share Memory=>', smemory);


let ret = smemory.CreateShareMemory('my-addon-test-memory', 64);
console.log('CreateShareMemory=>', ret);
//CreateShareMemory=> true


let writebuff = Buffer.alloc(10);
writebuff[0] = 9;
writebuff[1] = 7;
writebuff[2] = 5;
writebuff[3] = 3;
writebuff[4] = 1;
ret = smemory.WriteShareMemory('my-addon-test-memory', writebuff);
console.log('WriteShareMemory=>', ret);
//WriteShareMemory=> true


let readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemory('my-addon-test-memory', readbuff);
console.log('ReadShareMemory=> ', ret, readbuff);
//ReadShareMemory=>  true <Buffer 09 07 05 03 01 00 00 00 00 00>


ret = smemory.DeleteShareMemory('my-addon-test-memory');
console.log('DeleteShareMemory=> ', ret);
//DeleteShareMemory=>  true


readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemory('my-addon-test-memory', readbuff);
console.log('ReadShareMemory=> ', ret, readbuff);
//ReadShareMemory=>  false <Buffer 00 00 00 00 00 00 00 00 00 00>
```

## install
### nodejs
```bash
npm i share-memory-mac
```
### electron
```
npm i share-memory-mac
npm i --save-dev electron-rebuild
npm i --save-dev electron-prebuilt
.\node_modules\.bin\electron-rebuild.cmd -v 10.1.5/your electron version -w share-memory-mac
```

