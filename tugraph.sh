docker pull tugraph/tugraph-compile-centos7
docker run -d --name tugraph -p 7071:7071 -p 7072:7072 -it tugraph/tugraph-compile-centos7 bash
docker cp ../MV4PG tugraph:/
docker exec -it tugraph bash -c "./MV4PG/tugraph_test/tugraph_test.sh"