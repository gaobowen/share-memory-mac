const smemory = require('./index');
console.log('Init Share Memory=>', smemory);


const name = "share-memory-mac";

/*
open/create the file '/etc/sysctl.conf' write:

kern.sysv.shmmax=524288000
kern.sysv.shmmin=1
kern.sysv.shmmni=64
kern.sysv.shmseg=16
kern.sysv.semmns=130
kern.sysv.shmall=131072000
kern.sysv.maxproc=2048
kern.maxprocperuid=512

and then reboot.

to verify run this command

 sysctl kern.sysv.shmmax 
and it should give 524288000
=========================================
sysctl -a | grep -E "shmall|shmmax"
kern.sysv.shmmax: 4194304
kern.sysv.shmall: 1024
sysctl -w kern.sysv.shmmax=102400000
sysctl -a | grep -E "shmall|shmmax"
==========================================
//ipcs -as  //ipcrm -m [shm_id]
*/

let ret = smemory.CreateShareMemory(name, 1280 * 720 * 4 * 3);
console.log('CreateShareMemory=>', ret);

if (!ret) {
  console.log('shmmax require greater than 40,000k byte; also you can bash "sudo shmmax_set.sh && cat /etc/sysctl.conf" and reboot computer')
}

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

smemory.DeleteShareMemory(name);
