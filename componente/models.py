from django.db import models
from pathlib import Path


class Componente(models.Model):
    tipo       = models.ForeignKey('TipoDeComponente', on_delete=models.CASCADE, help_text="Tipo no qual o componente se enquadra")
    nome       = models.CharField(max_length=100, null=True, help_text="Nome do componente")
    foto       = models.ImageField(upload_to = 'fotosdecomponente', max_length=300, help_text="Foto do componente")
    quantidade = models.IntegerField(default=0)

    def __str__(self):
        return self.nome

class Categoria(models.Model):
    nome     = models.CharField(max_length=100, unique=True, null=True, help_text="Nome da categoria")
    quantidade = models.IntegerField(blank=True, null=True)

    def __str__(self):
        return self.nome

class TipoDeComponente(models.Model):
    nome     = models.CharField(max_length = 100, help_text="Nome do tipo de componente considerado")
    categoria  = models.ForeignKey('Categoria', on_delete=models.CASCADE, default="1", help_text="Categoria na qual o tipo de componente se enquadra")
    quantidade = models.IntegerField(blank=True, null=True)

    class Meta:
        verbose_name_plural = "Tipos de componentes"

    def __str__(self):
        return self.nome

