[Unit]
Description=labpass-seed
Requires=network-online.target
Requires=docker.service
After=network-online.target
After=docker.service

[Service]
Type=oneshot
Environment=IMAGE={{ docker_images.labpass_seed }}
ExecStartPre=-/usr/bin/docker kill %p
ExecStartPre=-/usr/bin/docker rm %p
ExecStartPre=-/usr/bin/docker pull ${IMAGE}
ExecStart=/usr/bin/docker run \
  --name %p \
  --network host \
  -e S6_KEEP_ENV=1 \
  -e NODE_ENV=production \
  -e NODE_ENV={{ labpass.node_env }} \
  -e GO_ENV={{ labpass.go_env }} \
  -e DATABASE_NAME={{ labpass.database_name }} \
  -e DATABASE_PASSWORD={{ labpass.database_password }} \
  -e DATABASE_HOST=127.0.0.1 \
  -e SESSION_SECRET={{ labpass.session_secret }} \
  -e JWT_SECRET={{ labpass.jwt_secret }} \
  ${IMAGE}
RemainAfterExit=true
ExecStop=/usr/bin/docker stop %p
ExecStopPost=-/usr/bin/docker rm %p
