## SRCSLICE FORK

[srcSlice](https://github.com/srcML/srcSlice) のフォークプロジェクトです。

## ビルド方法

```bash
# 事前に依存ライブラリをインストール

# リポジトリをクローン
# --recursive オプションをお忘れなく
$ git clone --recursive <repo>
$ cd <repo>

# リリースビルドの場合
# build-release/bin に生成物ができます
$ sh build-release.sh

# デバッグビルドの場合
# build-debug/bin に生成物ができます
$ sh build-debug.sh
```

### make レシピ

レシピ | 説明
------- | -------
srcSlice | srcslice のバイナリを生成
srcslicetest | srcslice のテストのバイナリを生成
(ex_srcslicetest) | srcslice の一部のテストを使いたい時に使いました

## 使用方法

```bash
# 事前にソースコードを srcML を用いて変換しておきます
# 標準出力にスライス結果が出ます
$ srcSlice <ソースコードをsrcMLで変換したもの(.xml)>
```

## 出力フォーマット

変数表と関数表を表すタブ区切りの文字列をまとめて JSON 形式で出力します。

```json
{
    "vars": "<変数表>",
    "funcs": "<関数表>"
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
