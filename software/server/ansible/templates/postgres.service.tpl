[Unit]
Description=postgres
Requires=network-online.target
Requires=docker.service
After=network-online.target
After=docker.service

[Service]
Restart=always
RestartSec=5
Environment=IMAGE={{docker_images.postgres}}
ExecStartPre=/usr/bin/docker pull ${IMAGE}
ExecStartPre=-/usr/bin/docker kill %p
ExecStartPre=-/usr/bin/docker rm %p
ExecStart=/usr/bin/docker run \
  --name %p \
  --network host \
  -v /var/lib/postgresql/data:/var/lib/postgresql/data \
  -e "PGDATA=/var/lib/postgresql/data" \
  -e "POSTGRES_PASSWORD={{ labpass.database_password }}" \
  ${IMAGE} --listen_addresses={{ postgres.bind_address }}

ExecStop=/usr/bin/docker stop %p
ExecStopPost=-/usr/bin/docker rm %p
ExecReload=/usr/bin/docker restart %p

[Install]
WantedBy=multi-user.target
