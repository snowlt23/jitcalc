# jitcalc

[N高等学校 Advent Calendar 2018 10日目](https://qiita.com/snowlt23/items/3ae2ae27726355fe814c)の記事で実装したJIT電卓言語です。

# ビルド

```sh
make jitcalc
make test
```

# ベンチマーク

```sh
sh bench.sh
```

|方式|実行時間|
|:--|--:|
|インタプリタ|5.55s|
|JIT|0.44s|