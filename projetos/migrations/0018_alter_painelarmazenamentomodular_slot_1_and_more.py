# Generated by Django 4.0 on 2023-03-30 19:49

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('componente', '0001_initial'),
        ('projetos', '0017_alter_painelarmazenamentomodular_slot_1_and_more'),
    ]

    operations = [
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_1',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot1', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_2',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot2', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_3',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot3', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_4',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot4', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_5',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot5', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_6',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot6', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_7',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot7', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_8',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot8', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='slot_9',
            field=models.OneToOneField(default='1', on_delete=django.db.models.deletion.CASCADE, related_name='slot9', to='componente.componente'),
        ),
    ]
