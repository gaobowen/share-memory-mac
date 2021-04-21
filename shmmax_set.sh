touch /etc/sysctl.conf

cat >/etc/sysctl.conf <<EOF
kern.sysv.shmmax=524288000
kern.sysv.shmmin=1
kern.sysv.shmmni=64
kern.sysv.shmseg=16
kern.sysv.semmns=130
kern.sysv.shmall=131072000
kern.sysv.maxproc=2048
kern.maxprocperuid=512
EOF