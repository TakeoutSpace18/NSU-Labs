//go:generate go tool oapi-codegen -config server-cfg.yaml ../../../api/worker.yaml
//go:generate go tool oapi-codegen -config models-cfg.yaml ../../../api/worker.yaml
//go:generate go tool oapi-codegen -config manager-client-cfg.yaml ../../../api/manager.yaml

package gen

