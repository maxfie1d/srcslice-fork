## SRCSLICE FORK

This repo is forked from [srcSlice](https://github.com/srcML/srcSlice).

## Build

```bash
# Install dependencies
# See BUILD.md

# Hey don't forget --recursive option
$ git clone --recursive <repo>
$ cd <repo>

# Release build
$ sh build-release.sh

# Debug build
$ sh build-debug.sh
```

### `make` recipes

Recipe | Description
------- | -------
srcSlice | Generate srcslice binary
srcslicetest | Generate srcslice's test binary
(ex_srcslicetest) | Used to partial test

## Usage

```bash
# Convert your source code using srcML first
# (Result would be sent as standard output)
$ srcSlice <source code covnerted by srcML>
```

## Output format

```json
{
    "vars": "<Variable table (tab-delimited)>",
    "funcs": "<Function table (tab-delimited)>",
    "controls": "<Control table (tab-delimited)>"
}
```

### 変数表のフォーマット

カラム名 | 説明
------- | -------
id | 変数に一意に振られる ID
file | 変数が宣言されたソースファイルのパス
func | 変数が宣言された関数。グローバル変数の場合は`__GLOBAL__`となる。
var | 変数名
def | 変数が代入された位置の集合
use | 変数が参照された位置の集合
dvars | 影響を与えた変数の集合。例えば `y = x;` のとき、 `x` の dvars は {`y`} である。
cfuncs | 引数として与えられた関数の集合

### 関数表のフォーマット

カラム名 | 説明
------- | -------
id | 関数に一意に振られる ID
func_name | 関数名
kind | 関数の種類(`user-defined` のみ)
file_path | 関数が宣言されたソースファイルのパス
declare_range | 関数が宣言されている範囲

## License

GPL (GNU General Public License)


## Resoruces

This project is made for my graduation thesis.
[See details](http://sel.ist.osaka-u.ac.jp/lab-db/Bthesis/index.html)
