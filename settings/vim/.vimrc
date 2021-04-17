""""""""""""""""""""""""""""""""
" Maintainer: 
"   krr <kriaeth@outlook.com>
"
""""""""""""""""""""""""""""""""

" === General Configutations ===

let mapleader = "\<space>"
nmap <leader>w :w!<cr>
nmap <leader>q :w!<cr>

" Remap <esc>
" setxbkmap -option caps:swapescape in terminal

filetype plugin on
filetype indent on

set encoding=utf-8


" === User Interface ===
set number
set linebreak
set showmatch
set novisualbell
set noerrorbells
set history=10000
set foldcolumn=1
set signcolumn=auto

" Current position
set ruler 

set lazyredraw

set expandtab
set shiftwidth=2
set tabstop=2


" === Encoding ===

set fileencoding=utf-8


" === Search ===

set ignorecase
set smartcase
set incsearch
set hlsearch


" === Fonts ===
syntax enable


" === Plugins ===

call plug#begin('~/.vim/plugged')
 
Plug 'junegunn/vim-easy-align'

Plug 'junegunn/fzf', { 'dir': '~/.fzf', 'do': './install --all' }

" Airline
Plug 'vim-airline/vim-airline'
Plug 'majutsushi/tagbar'
let g:airline#extensions#tagbar#enabled=1

" Airline theme
Plug 'vim-airline/vim-airline-themes'

" User-snippets
Plug 'sirver/ultisnips'
let g:UltiSnipsExpandTrigger = '<tab>'
let g:UltiSnipsJumpForwardTrigger = '<tab>'
let g:UltiSnipsJumpBackwardTrigger = '<s-tab>'

" Color Theme
Plug 'cocopon/iceberg.vim'

" Easy Motion
Plug 'easymotion/vim-easymotion'
map <leader> <Plug>(easymotion-prefix)

" Latex in Vim
Plug 'lervag/vimtex'

" Markdown for Vim
Plug 'godlygeek/tabular'
Plug 'plasticboy/vim-markdown'
let g:vim_markdown_math = 1

Plug 'iamcco/markdown-preview.nvim', { 'do': { -> mkdp#util#install() }, 'for': ['markdown', 'vim-plug']}

Plug 'neoclide/coc.nvim', {'branch': 'release'}

" Center the Text
Plug 'junegunn/goyo.vim'

" Nerd Tree
Plug 'preservim/nerdtree'
map <C-n> :NERDTreeToggle<cr>

" Autosave
Plug '907th/vim-auto-save'
let g:auto_save = 1

" Statify
Plug 'mhinz/vim-startify'

call plug#end()

colorscheme iceberg
