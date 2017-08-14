import paramiko

tc = paramiko.SSHClient()
tc.load_system_host_keys()
tc.connect(hostname='172.16.56.241',
           username='mengfanke',
           password='KPAbCkOXhyUW4')
tc.invoke_shell()
stdin, stdout, stderr = tc.exec_command('ls -l')
print(stdout.read(), stderr.read())
