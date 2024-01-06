#!/ebrmain/bin/run_script

TRUSTED_CLIENT_IP=192.168.1.22
PORT=8222

read_cfg_file() {
    while read x
    do
        x1=`echo $x|cut -d = -f 1|sed -e "s/[^a-zA-Z0-9_]//g"`
        x2=`echo $x|cut -d = -f 2-`
        eval ${2}${x1}='$x2'
    done < $1 || false
}

ensure_network_connected() {
    ifconfig eth0 > /dev/null 2>&1
    if [ $? -eq 0 ]; then
	return
    fi

    /ebrmain/bin/netagent status > /tmp/netagent_status_wb
    read_cfg_file /tmp/netagent_status_wb netagent_

    if [ "$netagent_nagtpid" -eq 0 ]; then
        echo "Network now disabled"
        /ebrmain/bin/dialog 5 "" @NeedInternet @Cancel @TurnOnWiFi
        want_connect=$?
        echo "want_connect=$want_connect"
        if ! [ "$want_connect" = 2 ]; then
            exit 1
        fi
        /ebrmain/bin/netagent net on
    fi
    /ebrmain/bin/netagent connect
}

ask_confirmation() {
    running_process_id=$(pgrep -o dropbear)
    if [ $? -eq 0 ]; then
	DLG_MESSAGE="SSH server is running.

To you want to stop SSH server?"

	dialog 2 "" "${DLG_MESSAGE}" @Cancel @OK
	if [ $? -ne 2 ]; then
	    exit 1
	else
	    kill -9 ${running_process_id}
	    exit 0
	fi
    fi

    DLG_MESSAGE="SSH server about to start.

Note that it will accept unauthenticated connections on port ${PORT} from the ${TRUSTED_CLIENT_IP} IP address."

    dialog 5 "" "${DLG_MESSAGE}" @Cancel @OK
    if [ $? -ne 2 ]; then
	exit 1
    fi
}

run_server() {
    dropbear -p ${PORT} -G ${TRUSTED_CLIENT_IP}
}

ask_confirmation
ensure_network_connected
run_server

# Local Variables:
# mode: sh;
# End:
