# Generated by Django 4.0 on 2023-03-18 00:12

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('componente', '0001_initial'),
        ('projetos', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo1',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo1', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo2',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo2', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo3',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo3', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo4',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo4', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo5',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo5', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo6',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo6', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo7',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo7', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo8',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo8', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='campo9',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='campo9', to='componente.componente'),
        ),
        migrations.AlterField(
            model_name='painelarmazenamentomodular',
            name='categoria',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='componente.categoria'),
        ),
    ]
