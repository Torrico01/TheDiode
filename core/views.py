from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.forms import UserCreationForm
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django.http import JsonResponse, HttpResponse
from django.urls import reverse
from datetime import datetime

from .forms import *
from componente.models import TipoDeComponente, Categoria, Componente
from core.config_variables import *
from projetos.models import *

import json, time

esp8266_ip = "192.168.43.21"
delayForSliderAnimation = 0.7

def visitor_ip_address(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[0]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip 

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
                

def painelDeArmazenamentoModular_json():
    dictJson = {}

    # Get pannels names
    for painel in PainelArmazenamentoModular.objects.all():
        dictJsonPainel = {}
        # Get components in pannels
        for i in range(1,10):
            if i==1: 
                keyName = painel.slot_1
                idx = painel.slot_1.id
            if i==2: 
                keyName = painel.slot_2
                idx = painel.slot_2.id
            if i==3: 
                keyName = painel.slot_3
                idx = painel.slot_3.id
            if i==4: 
                keyName = painel.slot_4
                idx = painel.slot_4.id
            if i==5: 
                keyName = painel.slot_5
                idx = painel.slot_5.id
            if i==6: 
                keyName = painel.slot_6
                idx = painel.slot_6.id
            if i==7: 
                keyName = painel.slot_7
                idx = painel.slot_7.id
            if i==8: 
                keyName = painel.slot_8
                idx = painel.slot_8.id
            if i==9: 
                keyName = painel.slot_9
                idx = painel.slot_9.id
            lim = keyName.limite
            qtd = keyName.quantidade
            keyName = str(i) + ". " + keyName.nome
            # Get slots proprieties
            dictJsonPainel.update({keyName:{"ID":idx,"Lim":lim,"Qtd":qtd}})
        dictJson.update(dictJsonPainel)
        
    # Define 'Funcionalidade' values
    dictJson["Funcionalidade"] = {}

    return(dictJson)
    
def painelDeArmazenamentoModular_splitIds(request, cookie_name):
    criar_painel_ids = request.COOKIES.get(cookie_name)
    if (criar_painel_ids != None): criar_painel_num_ids = len(criar_painel_ids.split(","))
    else: criar_painel_num_ids = 0
    return criar_painel_num_ids

def painelDeArmazenamentoModular_contextMsg(request):
    # Conta ids selecionados
    criar_painel_num_ids = painelDeArmazenamentoModular_splitIds(request, "criar_painel_ids")
    # Escreve mensagem da criação do painel
    if request.COOKIES.get('criar_painel') == "True" and criar_painel_num_ids == 9:
        criar_painel_msg = "todos os componentes selecionados"
    elif request.COOKIES.get('criar_painel') == "True":
        criar_painel_msg = str(criar_painel_num_ids) + " componentes selecionados"
    else: 
        criar_painel_msg = 0
    return criar_painel_msg


@csrf_exempt
def home(request):
    ip = visitor_ip_address(request)
    print(ip)
    #update_config()

    jsonDict = painelDeArmazenamentoModular_json()
    print(jsonDict)

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

    context = {}
    response = render(request, 'core/home.html', context)
    return response

def categorias(request):
    # Lista os componentes
    categorias = Categoria.objects.order_by('-quantidade')
    # (Se) Criando Painel de Armazenamento Modular:
    criar_painel_msg = painelDeArmazenamentoModular_contextMsg(request)
    # Return render
    context = {'categorias':categorias,
               'criar_painel_msg':criar_painel_msg}
    response = render(request, 'core/categorias.html', context)
    return response

def search(request):
    time.sleep(delayForSliderAnimation) # Wait for animation to finish
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

def tiposdecomponentes(request, id):
    # Lista os componentes
    tipodecomponente = TipoDeComponente.objects.filter(categoria=id).order_by('-quantidade')
    categorias = Categoria.objects.order_by('-quantidade')
    # (Se) Criando Painel de Armazenamento Modular:
    criar_painel_msg = painelDeArmazenamentoModular_contextMsg(request)
    # Return render
    context = {'categorias':categorias,
               'tipodecomponentes':tipodecomponente,
               'categoria_especifica':Categoria.objects.get(id=id),
               'id_categoria':id,
               'criar_painel_msg':criar_painel_msg}
    return render(request, 'core/tiposdecomponentes.html', context)

def componentes(request, id_categoria, id):
    # Lista os componentes
    componentes = Componente.objects.filter(tipo=id).order_by('nome')
    categorias = Categoria.objects.order_by('-quantidade')
    # (Se) Criando Painel de Armazenamento Modular:
    criar_painel_msg = painelDeArmazenamentoModular_contextMsg(request)
    # Return render
    context = {'categorias':categorias,
               'componentes': componentes,
               'tipo_especifico':TipoDeComponente.objects.get(id=id),
               'id_categoria':id_categoria,
               'id_tipo':id,
               'criar_painel_msg':criar_painel_msg}
    return render(request, 'core/componentes.html', context)

def modificar(request, id_categoria, id_tipo, id):
    componente_especifico = Componente.objects.get(id=id)
    tipo_especifico = TipoDeComponente.objects.get(id=id_tipo)
    form = ComponenteForm(instance=componente_especifico)
    if request.method == "POST":
        form = ComponenteForm(request.POST, instance=componente_especifico)
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            conta_componentes()
            update_config()
            return redirect("./")
    context = {'form': form,
               'componente_especifico': componente_especifico,
               'tipo_especifico': tipo_especifico}
    return render(request, 'core/modificar.html', context)

def criarcategoria(request):
    form = CriarCategoriaForm()
    if request.method == "POST":
        form = CriarCategoriaForm(request.POST)
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            return redirect('../')
    context = {'form': form}
    return render(request, 'core/criarcategoria.html', context)

def criartipo(request, id):
    form = CriarTipoForm(id)
    if request.method == "POST":
        form = CriarTipoForm(id,request.POST)
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            return redirect('../')
    context = {'form': form,
               'categoria_especifica':Categoria.objects.get(id=id)}
    return render(request, 'core/criartipo.html', context)

def criarcomponente(request, id_categoria, id):
    form = CriarComponenteForm(id)
    tipo_especifico = TipoDeComponente.objects.get(id=id)
    if request.method == "POST":
        form = CriarComponenteForm(id,request.POST)
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            conta_componentes()
            return redirect('../')
    context = {'form': form,
               'tipo_especifico':tipo_especifico}
    return render(request, 'core/criarcomponente.html', context)


def paineldearmazenamentomodular(request):
    criar_painel_num_ids = painelDeArmazenamentoModular_splitIds(request, "criar_painel_ids")
    if request.COOKIES.get('criar_painel') == "True" and criar_painel_num_ids == 9:
        criar_painel_completo = "True"
    else:
        criar_painel_completo = "False"
    context = {"criar_painel_completo":criar_painel_completo}
    return render(request, 'core/painelDeArmazenamentoMod.html', context)

def criarpaineldearmazenamentomodular(request):
    criar_painel_num_ids = painelDeArmazenamentoModular_splitIds(request, "criar_painel_ids")
    painel_component_selection_completed = request.COOKIES.get('criar_painel') == "True" and criar_painel_num_ids == 9
    if painel_component_selection_completed:
        form = CriarPainelForm(request.COOKIES.get("criar_painel_ids"))
        if request.method == "POST":
            form = CriarPainelForm(request.COOKIES.get("criar_painel_ids"),request.POST)
            if form.is_valid():
                form.save()
                if painel_component_selection_completed: criar_painel_completo = "True"
                else: criar_painel_completo = "False"
                context = {"criar_painel_completo":criar_painel_completo}
                response = render(request, 'core/painelDeArmazenamentoMod.html', context)
                response.set_cookie('criar_painel', "False") # Set cookie false
                response.delete_cookie('criar_painel_ids')
                return response
        context = {'form': form}
        response = render(request, 'core/criarPainelDeArmazenamentoMod.html', context)
    else:
        if painel_component_selection_completed: criar_painel_completo = "True"
        else: criar_painel_completo = "False"
        context = {"criar_painel_completo":criar_painel_completo}
        response = render(request, 'core/painelDeArmazenamentoMod.html', context)
        response.set_cookie('criar_painel', "True") # Set cookie true
    return response