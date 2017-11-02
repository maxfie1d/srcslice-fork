## ファイル構成

### src/tests

テストソースのあるディレクトリ。

* src/tests/samples/issue*
    * Issue番号に該当する現象を再現する


# メモ

```bash
# ビルドの実行
$ sh ./build.sh

# テストの実行
$ sh ./build.sh && ./test.sh
```

## トラブルシューティング

* サブモジュールの中身が消えた
    * ルートで`git submodule init && git submodule update`

# srcSlice
Lightweight tool for slicing

To build srcSlice:

1. Clone the repository with 'git clone --recursive' into the desired directory. Make sure you include the --recursive as srcSlice includes a submodule that must also be cloned.

2. Outside of the cloned directory, create a new directory for the build.  (This guide assumes the new directory is at the same directory tree level as the cloned directory)

3. Enter the new directory (not the cloned one) and type 'cmake ../{cloned directory}'

4. After cmake runs, simply type 'make' and all files should be built.  

5. Once everything is built, go into the 'bin' folder and that's where the executable will be.
