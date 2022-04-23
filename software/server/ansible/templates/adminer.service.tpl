[Unit]
Description=adminer
Requires=network-online.target
Requires=docker.service
After=network-online.target
After=docker.service

[Service]
Restart=always
RestartSec=5
Environment=IMAGE={{docker_images.adminer}}
ExecStartPre=/usr/bin/docker pull ${IMAGE}
ExecStartPre=-/usr/bin/docker kill %p
ExecStartPre=-/usr/bin/docker rm %p
ExecStart=/usr/bin/docker run \
  --user root \
  --name %p \
  --network host \
  ${IMAGE} \
  php -S 10.0.0.2:80 -t /var/www/html

ExecStop=/usr/bin/docker stop %p
ExecStopPost=-/usr/bin/docker rm %p
ExecReload=/usr/bin/docker restart %p

[Install]
WantedBy=multi-user.target
