url="https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs171.01/"
# wget -q $url -O - 2> /dev/null \
curl -s $url                    \
  | rg -i -o "pdf/.*\.pdf"      \
  | xargs -I% wget -q -N "$url/%" 2> /dev/null
