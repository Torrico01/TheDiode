# Generated by Django 4.2 on 2023-06-24 19:11

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        ('components', '__first__'),
    ]

    operations = [
        migrations.CreateModel(
            name='ModularStoragePanelBase',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('nome', models.CharField(max_length=200, null=True, unique=True)),
                ('display_oled', models.TextField(blank=True, null=True, verbose_name="(Output) Display OLED 0.96'")),
                ('chave_rotativa', models.CharField(blank=True, max_length=100, null=True, verbose_name='(Input) Chave rotativa')),
            ],
        ),
        migrations.CreateModel(
            name='ModularStoragePanel',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('nome', models.CharField(max_length=200, null=True, unique=True)),
                ('display_de_7_segmentos', models.FloatField(blank=True, null=True, verbose_name='(Output) Display de 7 segmentos e 4 dígitos')),
                ('matriz_de_leds', models.JSONField(blank=True, null=True, verbose_name='(Output) Matriz 3x3 de Leds RGB')),
                ('base', models.ForeignKey(default='1', on_delete=django.db.models.deletion.CASCADE, to='projects.modularstoragepanelbase')),
                ('slot_1', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot1', to='components.component')),
                ('slot_2', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot2', to='components.component')),
                ('slot_3', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot3', to='components.component')),
                ('slot_4', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot4', to='components.component')),
                ('slot_5', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot5', to='components.component')),
                ('slot_6', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot6', to='components.component')),
                ('slot_7', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot7', to='components.component')),
                ('slot_8', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot8', to='components.component')),
                ('slot_9', models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot9', to='components.component')),
            ],
        ),
    ]
