//go:generate go tool oapi-codegen -config server-cfg.yaml ../../../api/manager.yaml
//go:generate go tool oapi-codegen -config models-cfg.yaml ../../../api/manager.yaml
//go:generate go tool oapi-codegen -config worker-client-cfg.yaml ../../../api/worker.yaml

package gen

