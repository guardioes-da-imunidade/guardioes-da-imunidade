# Guia de configuração em ambiente de desenvolvimento

## 🐧 Linux / WSL

### Configuração de dependências

Instale as bibliotecas necessárias do Allegro 5.2:

```bash
sudo add-apt-repository ppa:allegro/5.2
sudo apt install "liballegro*5.2" "liballegro*5-dev"
```

Instale as bibliotecas opcionais para desenvolvimento:

```bash
sudo apt install gdb
```

### 🔄 Como rodar em modo desenvolvimento

Para executar o projeto com **hot reload**, use o script `watch.sh`:

```bash
# Dar permissão de execução (apenas na primeira vez)
chmod +x watch.sh

# Executar o modo desenvolvimento
./watch.sh
```

#### Como funciona o hot reload?

O sistema monitora mudanças nos arquivos `*.c` do projeto:

1. **Concatena** todo o conteúdo dos arquivos `*.c`
2. **Calcula o hash** dessa string (impressão digital do código)
3. **Verifica a cada 1 segundo** se o hash mudou
4. **Recompila automaticamente** usando o Makefile quando detecta mudanças

> ⚠️ **Importante**: É obrigatório dar permissão de execução ao script com `chmod +x watch.sh`

---
