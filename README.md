# bmp-conv

**bmp-conv** — library for bmp convolution.
*Convolution* — operation where you take the sum of products of elements from two 2D functions, 
where you let one of the two functions move over every element of the other function.

# Build

```shell
./scripts/build.sh
```

# Usage

```shell
./build/src/main bmps/source/nature.bmp bmps/result/nature.bmp id 1 0
```

# Test

```shell
./scripts/test.sh
```
> Required installed cmocka library.

## Thanks

Thanks [Mattflow](https://github.com/mattflow/cbmp?ysclid=m9104rn4ej835090391) for 
C library for reading, manipulating, and saving BMP images.

## License

Distributed under the [MIT License](https://choosealicense.com/licenses/mit/). See [`LICENSE`](LICENSE) for more
information.
