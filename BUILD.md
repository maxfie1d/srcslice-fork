# ビルド方法

## Ubuntu

```bash
$ git clone --recursive <repo>
$ cd <repo>

# 事前に libxml2, gtest, spdlog をインストールしておく

$ sh build-release.sh

# build-release/bin/ にバイナリが生成される
```

## Cygwin

1. リポジトリをクローン
1. libxml2がなければインストール(Cygwinのインストーラから入れられます)
1. プロジェクトルートの`CMakeLists.txt`冒頭を書き換え。(参考: http://moccosblue.blogspot.jp/2013/03/g-stdc11not-declared.html)

```cmake
# 追加
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ↓コメントアウト
# set(CMAKE_CXX_FLAGS "-std=c++11")
```
1. ロギングに[spdlog](https://github.com/gabime/spdlog)を使っているので、`src\lib\`に`spdlog/include/spdlog`をコピーする

1. `$ sh build-release.sh`
    * gtestをインストールしていないのでテストのビルドでエラーが出るが、build-release/bin/srcSlice.exeが出来上がっている
