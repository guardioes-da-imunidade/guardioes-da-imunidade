# Guia de configuração em ambiente de desenvolvimento

## WSL

### Configuração de dependências:

```bash
sudo add-apt-repository ppa:allegro/5.2
sudo apt install "liballegro*5.2" "liballegro*5-dev"
```

### Como rodar em modo desenvolvimento?

Para rodar efetivamente o projeto em modo de desenvolvimento, o script `watch.sh` deve ser executado no terminal (`./watch.sh`). A lógica por trás do _hot reload_ funciona concatenando todo o conteúdo dos arquivos `*.c` do projeto e calculando o _hash_ dessa string, criando uma espécie de impressão digital daquele momento do código. Quando algo mudar, o _while_ sem fim que roda a cada 1 segundo vai fazer o mesmo processo de calcular o _hash_ do conteúdo, e como algo mudou, o _hash_ gerado vai ser diferente e o jogo deve ser recompilado usando a ferramenta **Makefile**.

**Importante**: Para o arquivo `watch.sh` conseguir rodar corretamente, é preciso conceder permissão de execução: `chmod +x watch.sh`.
