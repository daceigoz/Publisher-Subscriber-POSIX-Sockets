1. Compilation is made with 'make'.
2. The RPC server is executed with './publications_server'.
3. The broker is executed with './broker -p port' which initializes the RPC server.
4. Now, either the editor with './editor -h host -p port -t "topic" -m "text"' or the suscriptor by 'java suscriptor -s server -p port' can be executed.
