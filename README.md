# share-memory-mac
a share memory module between diffrent process on windows. runtime nodejs/electron.

## example
```js
const smemory = require('./index');
console.log('Init Share Memory=>', smemory);

const name = "share-memory-mac";

let ret = smemory.CreateShareMemory(name, 1280 * 720 * 4 * 3);
console.log('CreateShareMemory=>', ret);

let writebuff = Buffer.alloc(10);

writebuff[0] = 1;
writebuff[1] = 3;
writebuff[2] = 5;
writebuff[3] = 7;
writebuff[4] = 9;
ret = smemory.WriteShareMemoryFast(name, writebuff, 1);
console.log('WriteShareMemoryFast =>', ret);

readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemoryFast(name, readbuff);
console.log('ReadShareMemoryFast => ', ret, readbuff);

readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemoryFast(name, readbuff, 3);
console.log('ReadShareMemoryFast => ', ret, readbuff);


ret = smemory.DeleteShareMemory(name);
console.log('DeleteShareMemory => ', ret);

readbuff = Buffer.alloc(10);
ret = smemory.ReadShareMemoryFast(name, readbuff);
console.log('ReadShareMemoryFast => ', ret, readbuff);

smemory.DeleteShareMemory(name, 1280 * 720 * 4 * 3);

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

