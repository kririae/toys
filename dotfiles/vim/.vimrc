""""""""""""""""""""""""""""""""
" Maintainer:
"   krr <kriaeth@outlook.com>
"
""""""""""""""""""""""""""""""""

" === General Configutations ===

let mapleader = "\<space>"
let maplocalleader = "\<space>"
nmap <leader>w :w!<cr>
nmap <leader>q :w!<cr>
cmap w!! w !sudo tee > /dev/null %
cmap s!! source ~/.vimrc
nnoremap <silent> <leader>f :Format<CR>

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
set mouse=a

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
set hlsearch


" === Fonts ===
syntax enable

" === Terminal ===
:tnoremap <Esc> <C-\><C-n>

" === Spell check ===
setlocal spell
set spelllang=en_us
inoremap <C-l> <c-g>u<Esc>[s1z=`]a<c-g>u

" === Plugins ===

call plug#begin('~/.vim/plugged')

Plug 'wakatime/vim-wakatime'

Plug 'junegunn/vim-easy-align'

Plug 'junegunn/fzf', { 'dir': '~/.fzf', 'do': './install --all' }

" Airline
Plug 'vim-airline/vim-airline'
Plug 'majutsushi/tagbar'
let g:airline#extensions#tabline#enabled = 1
let g:airline#extensions#tagbar#enabled = 1
let g:airline_powerline_fonts = 1

" Airline theme
Plug 'vim-airline/vim-airline-themes'

" User-snippets
Plug 'sirver/ultisnips'
let g:UltiSnipsExpandTrigger="<s-tab>"
let g:UltiSnipsJumpForwardTrigger="<s-tab>"
let g:UltiSnipsJumpBackwardTrigger="<s-tab>"
let g:ulti_expand_or_jump_res = 0 "default value, just set once

" Enter as trigger
function! Ulti_ExpandOrJump_and_getRes()
 call UltiSnips#ExpandSnippetOrJump()
 return g:ulti_expand_or_jump_res
endfunction
inoremap <CR> <C-R>=(Ulti_ExpandOrJump_and_getRes() > 0)?"":"\n"<CR>

" Color Theme
Plug 'sainnhe/edge'
Plug 'sainnhe/everforest'
Plug 'sainnhe/gruvbox-material'

" Easy Motion
Plug 'easymotion/vim-easymotion'
map <leader> <Plug>(easymotion-prefix)

" Auto-pairs
Plug 'jiangmiao/auto-pairs'

" Latex in Vim
Plug 'lervag/vimtex'

" Markdown for Vim
Plug 'godlygeek/tabular'
Plug 'plasticboy/vim-markdown'
let g:vim_markdown_math = 1

Plug 'iamcco/markdown-preview.nvim', { 'do': { -> mkdp#util#install() }, 'for': ['markdown', 'vim-plug']}

" Center the Text
Plug 'junegunn/goyo.vim'

" Nerd Tree
Plug 'preservim/nerdtree'
map <C-n> :NERDTreeToggle<cr>

" Autosave
Plug '907th/vim-auto-save'
let g:auto_save = 1

Plug 'justinmk/vim-sneak'

" Statify
Plug 'mhinz/vim-startify'

" Tmuxline
Plug 'edkolev/tmuxline.vim'

if has('vim')
  " Coc.nvim settings
  Plug 'neoclide/coc.nvim', {'branch': 'release'}
  " Enable <tab> for selecting
  function! s:check_back_space() abort
      let col = col('.') - 1
      return !col || getline('.')[col - 1]    =~ '\s'
  endfunction
  inoremap <silent><expr> <tab>
        \ pumvisible() ? "\<C-n>" :
        \ <SID>check_back_space() ? "\<tab>" :
        \ coc#refresh()
  inoremap <expr><S-TAB>
        \ pumvisible() ? "\<C-p>" : "\<C-h>"
elseif has('nvim')
  " Nvim LSP configuration
  Plug 'neovim/nvim-lspconfig'
  Plug 'williamboman/nvim-lsp-installer'
  Plug 'hrsh7th/cmp-nvim-lsp'
  Plug 'hrsh7th/cmp-buffer'
  Plug 'hrsh7th/cmp-path'
  Plug 'hrsh7th/cmp-cmdline'
  Plug 'hrsh7th/nvim-cmp'
  Plug 'quangnguyen30192/cmp-nvim-ultisnips'
endif

Plug 'lukas-reineke/format.nvim'
call plug#end()

" Start NERDTree when Vim starts with a directory argument.
" autocmd StdinReadPre * let s:std_in=1
" autocmd VimEnter * if argc() == 1 && isdirectory(argv()[0]) && !exists('s:std_in') |
"    \ execute 'NERDTree' argv()[0] | wincmd p | enew | execute 'cd '.argv()[0] | endif

" Start NERDTree and put the cursor back in the other window.
" autocmd VimEnter * NERDTree | wincmd p

" Exit Vim if NERD Tree is the only window left.
autocmd BufEnter * if tabpagenr('$') == 1 && winnr('$') == 1 && exists('b:NERDTree') && b:NERDTree.isTabTree() |
    \ quit | endif

set clipboard=unnamedplus

" Tex settings
let g:tex_flavor = 'latex'
let g:vimtex_quickfix_mode = 0
let g:vimtex_view_general_viewer = 'zathura'
let g:vimtex_view_method = 'zathura'
let g:vimtex_compiler_progname = 'nvr'
set conceallevel=1
let g:tex_conceal='abdmg'

" Theme configuration
let g:airline_theme = 'everforest'
if exists('+termguicolors')
  let &t_8f = "\<Esc>[38;2;%lu;%lu;%lum"
  let &t_8b = "\<Esc>[48;2;%lu;%lu;%lum"
  set termguicolors
endif
set background=dark
let g:everforest_background = 'hard'
colorscheme everforest

set completeopt=menu,menuone,noselect
lua << EOF
require"format".setup {
  ["*"] = {{cmd = {"sed -i 's/[ \t]*$//'"}}},
  vim = {
    {
      cmd = {"lua-format --indent-width=2 --tab-width=2 --no-use-tab -i"},
      start_pattern = "^lua << EOF$",
      end_pattern = "^EOF$"
    }
  }
  -- tex = {cmd = {"latexindent -w"}} // currenrly not configured
}

local cmp = require "cmp"
cmp.setup({
  snippet = {
    -- Snippet engine
    expand = function(args) vim.fn["UltiSnips#Anon"](args.body) end
  },
  mapping = {
    ["<Tab>"] = cmp.mapping({
      c = function()
        if cmp.visible() then
          cmp.select_next_item({behavior = cmp.SelectBehavior.Insert})
        else
          cmp.complete()
        end
      end,
      i = function(fallback)
        if cmp.visible() then
          cmp.select_next_item({behavior = cmp.SelectBehavior.Insert})
        elseif vim.fn["UltiSnips#CanJumpForwards"]() == 1 then
          vim.api.nvim_feedkeys(t("<Plug>(ultisnips_jump_forward)"), "m", true)
        else
          fallback()
        end
      end,
      s = function(fallback)
        if vim.fn["UltiSnips#CanJumpForwards"]() == 1 then
          vim.api.nvim_feedkeys(t("<Plug>(ultisnips_jump_forward)"), "m", true)
        else
          fallback()
        end
      end
    }),
    ["<S-Tab>"] = cmp.mapping({
      c = function()
        if cmp.visible() then
          cmp.select_prev_item({behavior = cmp.SelectBehavior.Insert})
        else
          cmp.complete()
        end
      end,
      i = function(fallback)
        if cmp.visible() then
          cmp.select_prev_item({behavior = cmp.SelectBehavior.Insert})
        elseif vim.fn["UltiSnips#CanJumpBackwards"]() == 1 then
          return vim.api.nvim_feedkeys(t("<Plug>(ultisnips_jump_backward)"),
                                       "m", true)
        else
          fallback()
        end
      end,
      s = function(fallback)
        if vim.fn["UltiSnips#CanJumpBackwards"]() == 1 then
          return vim.api.nvim_feedkeys(t("<Plug>(ultisnips_jump_backward)"),
                                       "m", true)
        else
          fallback()
        end
      end
    }),
    ["<Down>"] = cmp.mapping(cmp.mapping.select_next_item({
      behavior = cmp.SelectBehavior.Select
    }), {"i"}),
    ["<Up>"] = cmp.mapping(cmp.mapping.select_prev_item({
      behavior = cmp.SelectBehavior.Select
    }), {"i"}),
    ["<C-n>"] = cmp.mapping({
      c = function()
        if cmp.visible() then
          cmp.select_next_item({behavior = cmp.SelectBehavior.Select})
        else
          vim.api.nvim_feedkeys(t("<Down>"), "n", true)
        end
      end,
      i = function(fallback)
        if cmp.visible() then
          cmp.select_next_item({behavior = cmp.SelectBehavior.Select})
        else
          fallback()
        end
      end
    }),
    ["<C-p>"] = cmp.mapping({
      c = function()
        if cmp.visible() then
          cmp.select_prev_item({behavior = cmp.SelectBehavior.Select})
        else
          vim.api.nvim_feedkeys(t("<Up>"), "n", true)
        end
      end,
      i = function(fallback)
        if cmp.visible() then
          cmp.select_prev_item({behavior = cmp.SelectBehavior.Select})
        else
          fallback()
        end
      end
    }),
    ["<C-b>"] = cmp.mapping(cmp.mapping.scroll_docs(-4), {"i", "c"}),
    ["<C-f>"] = cmp.mapping(cmp.mapping.scroll_docs(4), {"i", "c"}),
    ["<C-Space>"] = cmp.mapping(cmp.mapping.complete(), {"i", "c"}),
    ["<C-e>"] = cmp.mapping({i = cmp.mapping.close(), c = cmp.mapping.close()}),
    ["<CR>"] = cmp.mapping({
      i = cmp.mapping.confirm({
        behavior = cmp.ConfirmBehavior.Replace,
        select = false
      }),
      c = function(fallback)
        if cmp.visible() then
          cmp.confirm({behavior = cmp.ConfirmBehavior.Replace, select = false})
        else
          fallback()
        end
      end
    })
  },
  sources = cmp.config.sources({{name = "nvim_lsp"}, {name = "ultisnips"}},
                               {{name = "buffer"}})
})

local capabilities = require("cmp_nvim_lsp").update_capabilities(vim.lsp
                                                                     .protocol
                                                                     .make_client_capabilities())
require"lspconfig".texlab.setup {
  cmd = {"/home/kr2/.local/share/nvim/lsp_servers/latex/texlab"},
  capabilities = capabilities
}
EOF

