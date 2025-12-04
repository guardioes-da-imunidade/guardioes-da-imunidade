
# 🎮 Guardiões da Imunidade

Jogo 2D do gênero **Tower Defense**, desenvolvido em **C** utilizando a biblioteca **Allegro 5**, como parte do projeto integrador do **2º semestre do curso de Ciência da Computação**.

## :scroll: História

Dentro de um vasto universo microscópico, **uma guerra silenciosa acontece a cada instante**. De um lado, as **células de defesa** atuam como guardiãs incansáveis, sempre em alerta para proteger o organismo contra qualquer ameaça. Cada uma cumpre sua função essencial, patrulhando como sentinelas e usando seu poder para manter o corpo em equilíbrio.

Do outro lado, **microrganismos invasores** elaboram novas estratégias para romper as barreiras naturais e conquistar território. Rápidos e numerosos, tentam transformar o corpo em seu novo lar, espalhando infecção por onde passam.

Mas um novo perigo surge: **um vírus desconhecido**, mais poderoso, inteligente e adaptável do que todos os anteriores. Capaz de multiplicar-se rapidamente e até fortalecer outros vírus, **ele desencadeia um caos crescente** e coloca todo o sistema imunológico sob ameaça.

Agora, apenas as **células protetoras** podem impedir que essa super-ameaça tome controle completo do organismo — **e cabe a você liderá-las nessa batalha pela sobrevivência**.

---

## 📋 Pré-requisitos

### 🪟 Windows
- **Visual Studio 2022** (Community, Professional ou Enterprise)
- **Windows 10/11** (x64 ou x86)

### 🐧 Linux (Ubuntu)
- **GCC** (compilador C)
- **Make** (ferramenta de build)
- **Allegro 5.2** (biblioteca gráfica)

---

## 🪟 Windows - Visual Studio 2022

### ⚙️ Opção 1: Clonar pelo Visual Studio

1. Abra o **Visual Studio 2022**
2. Na tela inicial, clique em **“Clonar um repositório”**
3. Cole a URL:
   ```bash
   https://github.com/guardioes-da-imunidade/guardioes-da-imunidade.git

4. Escolha a pasta de destino e clique em **“Clonar”**

---

### 📦 Configuração do Projeto

1. **Abrir a Solução**

   * Abra o arquivo **`GuardioesImunidade.sln`** no Visual Studio

2. **Restaurar Pacotes NuGet (Allegro)**

   * O Visual Studio exibirá uma barra amarela solicitando restauração
   * Clique em **“Restaurar”**
   * Ou use o console do NuGet:

     ```powershell
     Update-Package -reinstall
     ```

3. **Verificar Configuração**

   * Na barra superior, selecione:

     * **Configuração**: `Debug`
     * **Plataforma**: `x64`

4. **Compilar e Executar**

   * **F5** - Executar com depuração
   * **Ctrl+F5** - Executar sem depuração
   * **Ctrl+Shift+B** - Apenas compilar

---

### 📚 Bibliotecas Allegro Incluídas

* Allegro Core 5.2
* Primitives Addon
* Image Addon
* Font Addon
* TTF Addon
* Audio Addon
* Audio Codec Addon
* Dialog Addon
* Color Addon

---

## 🐧 Linux (Ubuntu)

### ⚙️ Configuração de Dependências

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

---

### 🧩 Como Funciona o Hot Reload

O sistema monitora mudanças nos arquivos `*.c` do projeto:

1. **Concatena** todo o conteúdo dos arquivos `*.c`
2. **Calcula o hash** dessa string (impressão digital do código)
3. **Verifica a cada 1 segundo** se o hash mudou
4. **Recompila automaticamente** usando o Makefile quando detecta mudanças

> ⚠️ **Importante:** É obrigatório dar permissão de execução ao script com
> `chmod +x watch.sh`

---
