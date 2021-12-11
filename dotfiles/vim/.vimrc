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
let g:UltiSnipsExpandTrigger = '<Plug>(ultisnips_expand)'
let g:UltiSnipsJumpForwardTrigger = '<Plug>(ultisnips_jump_forward)'
let g:UltiSnipsJumpBackwardTrigger = '<Plug>(ultisnips_jump_backward)'
let g:UltiSnipsListSnippets = '<c-x><c-s>'
let g:UltiSnipsRemoveSelectModeMappings = 0

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

" Format configuration.
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

" set clipboard=unnamedplus

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
        select = true
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

-- Use buffer source for `/`.
cmp.setup.cmdline('/', {
  completion = {autocomplete = false},
  sources = {
    -- { name = 'buffer' }
    {name = 'buffer', opts = {keyword_pattern = [=[[^[:blank:]].*]=]}}
  }
})

-- Use cmdline & path source for ':'.
cmp.setup.cmdline(':', {
  completion = {autocomplete = false},
  sources = cmp.config.sources({{name = 'path'}}, {{name = 'cmdline'}})
})

local capabilities = require("cmp_nvim_lsp").update_capabilities(vim.lsp
                                                                     .protocol
                                                                     .make_client_capabilities())

-- LSP configurations.
local on_attach = function(client, bufnr)
  local function buf_set_keymap(...) vim.api.nvim_buf_set_keymap(bufnr, ...) end
  local function buf_set_option(...) vim.api.nvim_buf_set_option(bufnr, ...) end

  -- Enable completion triggered by <c-x><c-o>
  buf_set_option('omnifunc', 'v:lua.vim.lsp.omnifunc')

  -- Mappings.
  local opts = {noremap = true, silent = true}

  -- See `:help vim.lsp.*` for documentation on any of the below functions
  buf_set_keymap('n', 'gD', '<cmd>lua vim.lsp.buf.declaration()<CR>', opts)
  buf_set_keymap('n', 'gd', '<cmd>lua vim.lsp.buf.definition()<CR>', opts)
  buf_set_keymap('n', 'K', '<cmd>lua vim.lsp.buf.hover()<CR>', opts)
  buf_set_keymap('n', 'gi', '<cmd>lua vim.lsp.buf.implementation()<CR>', opts)
  buf_set_keymap('n', '<C-k>', '<cmd>lua vim.lsp.buf.signature_help()<CR>', opts)
  buf_set_keymap('n', '<space>wa',
                 '<cmd>lua vim.lsp.buf.add_workspace_folder()<CR>', opts)
  buf_set_keymap('n', '<space>wr',
                 '<cmd>lua vim.lsp.buf.remove_workspace_folder()<CR>', opts)
  buf_set_keymap('n', '<space>wl',
                 '<cmd>lua print(vim.inspect(vim.lsp.buf.list_workspace_folders()))<CR>',
                 opts)
  buf_set_keymap('n', '<space>D', '<cmd>lua vim.lsp.buf.type_definition()<CR>',
                 opts)
  buf_set_keymap('n', '<space>rn', '<cmd>lua vim.lsp.buf.rename()<CR>', opts)
  buf_set_keymap('n', '<space>ca', '<cmd>lua vim.lsp.buf.code_action()<CR>',
                 opts)
  buf_set_keymap('n', 'gr', '<cmd>lua vim.lsp.buf.references()<CR>', opts)
  buf_set_keymap('n', '<space>e', '<cmd>lua vim.diagnostic.open_float()<CR>',
                 opts)
  buf_set_keymap('n', '[d', '<cmd>lua vim.diagnostic.goto_prev()<CR>', opts)
  buf_set_keymap('n', ']d', '<cmd>lua vim.diagnostic.goto_next()<CR>', opts)
  buf_set_keymap('n', '<space>q', '<cmd>lua vim.diagnostic.setloclist()<CR>',
                 opts)
  buf_set_keymap('n', '<space>f', '<cmd>lua vim.lsp.buf.formatting()<CR>', opts)
end

require"lspconfig".texlab.setup {
  cmd = {"/home/kr2/.local/share/nvim/lsp_servers/latex/texlab"},
  capabilities = capabilities,
  on_attach = on_attach
}
EOF
