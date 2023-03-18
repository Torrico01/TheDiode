from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.forms import UserCreationForm
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django.http import JsonResponse, HttpResponse
from django.urls import reverse
from datetime import datetime

from .forms import CriarCategoriaForm, CriarTipoForm, CriarComponenteForm, ComponenteForm
from componente.models import TipoDeComponente, Categoria, Componente
from core.config_variables import *

import json

esp8266_ip = "192.168.43.21"

def conta_componentes():
    categorias = Categoria.objects.all()
    count_total = 0
    count = 0

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

def update_config():
    jsonFile = open(PAINEL_CONFIG_JSON)
    dictEspConfig = json.load(jsonFile)

    for categoria in dictEspConfig:
        if categoria != "Funcionalidades":
            for componente in dictEspConfig[categoria]:
                try:
                    componente_id = dictEspConfig[categoria][componente]["ID"].split("/")[2]
                    componente_obj = Componente.objects.get(id=int(componente_id))
                    dictEspConfig[categoria][componente]["Quantidade"] = componente_obj.quantidade
                except:
                    continue

    jsonEspConfig = json.dumps(dictEspConfig, indent=4, sort_keys=True)
    with open(PAINEL_CONFIG_JSON, "w") as jsonFile:
        jsonFile.write(jsonEspConfig)
                


def visitor_ip_address(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[0]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip 


@csrf_exempt
def home(request):
    ip = visitor_ip_address(request)
    print(ip)
    #update_config()

    if request.method == "POST" and ip == esp8266_ip:
        # Read json request data
        s = request.body.decode('utf8')
        print(s)
        '''
        in_data = json.loads(s)
        jsonEspConfig = json.dumps(in_data, indent=4, sort_keys=True)
        with open(PAINEL_CONFIG_JSON, "w") as jsonFile:
            jsonFile.write(jsonEspConfig)

        print("In ESP Configuration Data:")
        print(jsonEspConfig)
        '''

        # Returns empty json dictionary
        return JsonResponse({})

    if request.method == "GET" and ip == esp8266_ip:
        # MANDAR PARA O ESP O MODULO ATUAL DO DB, O PEDAÇO DO JSON CORRESPONDENTE, E O RESTANTE DAS INFOS

        # Read .json file as dictionary
        jsonFile = open(PAINEL_CONFIG_JSON)
        dictEspConfig = json.load(jsonFile)

        # Get current time and update .json file
        horaAtual = datetime.now().strftime("%H%M") 
        dictEspConfig["Funcionalidades"]["Horario"]["Atual"] = horaAtual
        jsonEspConfig = json.dumps(dictEspConfig, indent=4, sort_keys=True)
        with open(PAINEL_CONFIG_JSON, "w") as jsonFile:
            jsonFile.write(jsonEspConfig)

        #print("Out ESP Configuration Data:")
        #print(jsonEspConfig)
        #out_hora = {"hora":datetime.now().strftime("%H%M")}

        # Returns json dictionary
        print(dictEspConfig["Capacitor eletrolitico 1"])
        return JsonResponse(dictEspConfig["Capacitor eletrolitico 1"])

    categorias = Categoria.objects.order_by('-quantidade')
    context = {'categorias':categorias}
    return render(request, 'core/menu.html', context)


def search(request):
    q=request.GET['q']
    tipodecomponentes = TipoDeComponente.objects.filter(nome__icontains=q).order_by('nome')
    componentes = Componente.objects.none()
    if (len(tipodecomponentes) == 0):
        componentes = Componente.objects.filter(nome__icontains=q).order_by('nome')
    else:
        for tipodecomponente in tipodecomponentes:
            componentes |= tipodecomponente.componente_set.all()
    categorias = Categoria.objects.order_by('-quantidade')
    context = {'componentes':componentes, 
               'categorias':categorias, 
               'tipodecomponentes':tipodecomponentes}
    return render(request, 'core/search.html', context)

def tipos_de_componentes(request, id):
    tipodecomponente = TipoDeComponente.objects.filter(categoria=id).order_by('-quantidade')
    categorias = Categoria.objects.order_by('-quantidade')
    context = {'categorias':categorias,
               'tipodecomponentes':tipodecomponente,
               'categoria_especifica':Categoria.objects.get(id=id),
               'id_categoria':id}
    return render(request, 'core/tipos_de_componentes.html', context)

def componentes(request, id_categoria, id):
    componentes = Componente.objects.filter(tipo=id).order_by('nome')
    categorias = Categoria.objects.order_by('-quantidade')
    context = {'categorias':categorias,
               'componentes': componentes,
               'tipo_especifico':TipoDeComponente.objects.get(id=id),
               'id_categoria':id_categoria,
               'id_tipo':id}
    return render(request, 'core/componentes.html', context)

def modificar(request, id_categoria, id_tipo, id):
    componente_especifico = Componente.objects.get(id=id) 
    tipo_especifico = TipoDeComponente.objects.get(id=id_tipo)
    form = ComponenteForm(instance=componente_especifico)
    if request.method == "POST":
        form = ComponenteForm(request.POST, instance=componente_especifico)
        if form.is_valid():
            form.save()
            conta_componentes()
            update_config()
            return redirect('../../' + str(id_categoria) + '/' + str(id_tipo))
    context = {'form': form,
               'componente_especifico': componente_especifico,
               'tipo_especifico': tipo_especifico}
    return render(request, 'core/modificar.html', context)

def criarcategoria(request):
    form = CriarCategoriaForm()
    if request.method == "POST":
        form = CriarCategoriaForm(request.POST)
        if form.is_valid():
            form.save()
            return redirect('../')
    context = {'form': form}
    return render(request, 'core/criarcategoria.html', context)

def criartipo(request, id):
    form = CriarTipoForm(id)
    if request.method == "POST":
        form = CriarTipoForm(id,request.POST)
        if form.is_valid():
            form.save()
            return redirect('../../' + str(id))
    context = {'form': form,
               'categoria_especifica':Categoria.objects.get(id=id)}
    return render(request, 'core/criartipo.html', context)

def criarcomponente(request, id_categoria, id):
    form = CriarComponenteForm(id)
    tipo_especifico = TipoDeComponente.objects.get(id=id)
    if request.method == "POST":
        form = CriarComponenteForm(id,request.POST)
        if form.is_valid():
            form.save()
            conta_componentes()
            return redirect('../../../' + str(tipo_especifico.categoria.id) + '/' + str(id))
    context = {'form': form,
               'tipo_especifico':tipo_especifico}
    return render(request, 'core/criarcomponente.html', context)


