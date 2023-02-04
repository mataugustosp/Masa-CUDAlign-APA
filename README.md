# Masa-Cudalign-APA

Este trabalho de graduação apresenta o Módulo de Alinhamento com Poda Agilizada (APA) que, integrado à ferramenta MASA-CUDAlign para 1 GPU, modifica a otimização de descarte de blocos Block Pruning, buscando agilizar a poda e  aumentar área de descarte da matriz de programação dinâmica. Com esse fim, utiliza-se o alinhador heurístico BLAST para a geração de um escore inicial a ser inserido automaticamente no MASA-CUDAlign, que inicialmente tinha escore 0.

### Donwload 

Latest Version: 

### Compiling CUDAlign 

```console
cd masa-cudalign-apa/masa-cudalign
./configure 
make 
```

### Compiling APA Module

No diretório masa-cudalign-apa: 

```console
  g++ modulo_apa.cpp -o apa 
```

### Executing APA Module

```console
  ./apa [options] seq1.fasta seq2.fasta 
```

  
