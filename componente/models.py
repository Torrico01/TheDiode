from django.db import models
from pathlib import Path

class Categoria(models.Model):
    nome       = models.CharField(max_length=100, unique=True, null=True)
    quantidade = models.IntegerField(blank=True, null=True)

    def __str__(self):
        return self.nome

class TipoDeComponente(models.Model):
    nome       = models.CharField(max_length = 100)
    categoria  = models.ForeignKey('Categoria', on_delete=models.CASCADE, default="1")
    quantidade = models.IntegerField(blank=True, null=True)

    class Meta:
        verbose_name_plural = "Tipos de componentes"

    def __str__(self):
        return self.nome

class Componente(models.Model):
    nome       = models.CharField(max_length=100, null=True)
    tipo       = models.ForeignKey('TipoDeComponente', on_delete=models.CASCADE, default="1")
    quantidade = models.IntegerField(default=0)
    limite     = models.IntegerField(default=99, null=True)

    def __str__(self):
        return self.nome
