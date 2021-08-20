from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.models import User
from django.urls import reverse

from .forms import CriarCategoriaForm, CriarTipoForm, CriarComponenteForm, ComponenteForm
from componente.models import TipoDeComponente, Categoria, Componente

def conta_componentes():
    categorias = Categoria.objects.all()
    count = 0
    count_total = 0
    for categoria in categorias:
        tipodecomponentes_filtrados = categoria.tipodecomponente_set.all()
        for tipodecomponente in tipodecomponentes_filtrados:
            componentes_filtrados = tipodecomponente.componente_set.all()
            for componente_filtrado in componentes_filtrados:
                count += componente_filtrado.quantidade
            tipodecomponente.quantidade = count
            tipodecomponente.save()
            count_total += count
            count = 0
        categoria.quantidade = count_total
        categoria.save()
        count_total = 0

def home(request):
    conta_componentes()
    categorias = Categoria.objects.all()
    context = {'categorias':categorias}
    return render(request, 'core/menu.html', context)

def search(request):
    conta_componentes()
    q=request.GET['q']
    tipodecomponentes = TipoDeComponente.objects.filter(nome__icontains=q).order_by('-id')
    if (len(tipodecomponentes) == 0):
        componentes = Componente.objects.filter(nome__icontains=q).order_by('-id')
    else:
        for tipodecomponente in tipodecomponentes:
            componentes = tipodecomponente.componente_set.all()
    tipodecomponentes = TipoDeComponente.objects.filter(nome__icontains=q).order_by('-id')
    categorias = Categoria.objects.all()
    context = {'componentes':componentes, 
               'categorias':categorias, 
               'tipodecomponentes':tipodecomponentes}
    return render(request, 'core/search.html', context)

def tipos_de_componentes(request, id):
    conta_componentes()
    tipodecomponente = TipoDeComponente.objects.filter(categoria=id)
    categorias = Categoria.objects.all()
    context = {'categorias':categorias, 
               'tipodecomponentes':tipodecomponente,
               'categoria_especifica':Categoria.objects.get(id=id),
               'id_categoria':id}
    return render(request, 'core/tipos_de_componentes.html', context)

def componentes(request, id_categoria, id):
    conta_componentes()
    componentes = Componente.objects.filter(tipo=id)
    categorias = Categoria.objects.all()
    context = {'componentes': componentes, 
               'categorias':categorias,
               'tipo_especifico':TipoDeComponente.objects.get(id=id),
               'id_categoria':id_categoria,
               'id_tipo':id}
    return render(request, 'core/componentes.html', context)

def modificar(request, id_categoria, id_tipo, id):
    conta_componentes()
    componente_especifico = Componente.objects.get(id=id)
    tipo_especifico = TipoDeComponente.objects.get(id=id_tipo)
    categorias = Categoria.objects.all()
    form = ComponenteForm(instance=componente_especifico)
    if request.method == "POST":
        form = ComponenteForm(request.POST, instance=componente_especifico)
        if form.is_valid():
            form.save()
            return redirect('../../' + str(id_categoria) + '/' + str(id_tipo))
    context = {'form': form,
               'componente_especifico': componente_especifico,
               'tipo_especifico': tipo_especifico,
               'categorias':categorias}
    return render(request, 'core/modificar.html', context)

def criarcategoria(request):
    conta_componentes()
    form = CriarCategoriaForm()
    categorias = Categoria.objects.all()
    if request.method == "POST":
        form = CriarCategoriaForm(request.POST)
        if form.is_valid():
            form.save()
            return redirect('../')
    context = {'form': form,
               'categorias':categorias}
    return render(request, 'core/criarcategoria.html', context)

def criartipo(request, id):
    conta_componentes()
    form = CriarTipoForm(id)
    categorias = Categoria.objects.all()
    if request.method == "POST":
        form = CriarTipoForm(id,request.POST)
        if form.is_valid():
            form.save()
            return redirect('../../' + str(id))
    context = {'form': form,
               'categorias':categorias,
               'categoria_especifica':Categoria.objects.get(id=id)}
    return render(request, 'core/criartipo.html', context)

def criarcomponente(request, id_categoria, id):
    conta_componentes()
    form = CriarComponenteForm(id)
    tipo_especifico = TipoDeComponente.objects.get(id=id)
    categorias = Categoria.objects.all()
    if request.method == "POST":
        form = CriarComponenteForm(id,request.POST)
        if form.is_valid():
            form.save()
            return redirect('../../../' + str(tipo_especifico.categoria.id) + '/' + str(id))
    context = {'form': form,
               'categorias':categorias,
               'tipo_especifico':tipo_especifico}
    return render(request, 'core/criarcomponente.html', context)


