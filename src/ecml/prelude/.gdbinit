set env      DHSHOME=/home/ma/mak/Dropbox/work/odb_server_bundle/server/src/odbsvr/test/dhshome
set env _TEST_ECKIT_HOME=/home/ma/mak/Dropbox/work/odb_server_bundle/server/src/odbsvr/test/dhshome
set env TEST_DHSHOME=/home/ma/mak/Dropbox/work/odb_server_bundle/server/src/odbsvr/test/dhshome
set env TEST_DATA_DIRECTORY=/tmp/build/bundle/debug/odb_api/tests/

file /tmp/build/bundle/debug/bin/eckit_ecml_test

break main
run
catch throw
break /home/ma/mak/Dropbox/work/odb_server_bundle/eckit/src/experimental/eckit/ecml/prelude/REPLHandler.cc:87
#break  eckit::REPLHandler::completion
continue

