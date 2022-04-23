[Unit]
Description=labpass
Requires=network-online.target
Requires=docker.service
After=network-online.target
After=docker.service

[Service]
Restart=always
RestartSec=5
Environment=IMAGE={{docker_images.labpass}}
EnvironmentFile=/etc/labpass/passwords
ExecStartPre=-/usr/bin/docker kill %p
ExecStartPre=-/usr/bin/docker rm %p
ExecStart=/usr/bin/docker run \
  --name %p \
  --network host \
  -e ADDR=10.0.0.1 \
  -e PORT=80 \
  -e HOST=server.labpass.epl \
  -e S6_KEEP_ENV=1 \
  -e NODE_ENV=production \
  -e GO_ENV=production \
  -e DATABASE_NAME=labpass_server_development \
  -e DATABASE_PASSWORD= \
  -e DATABASE_HOST=db.labpass.epl \
  -e SESSION_SECRET \
  -e JWT_SECRET \
  ${IMAGE}
ExecStop=/usr/bin/docker stop %p
ExecStopPost=-/usr/bin/docker rm %p
ExecReload=/usr/bin/docker restart %p

[Install]
WantedBy=multi-user.target
