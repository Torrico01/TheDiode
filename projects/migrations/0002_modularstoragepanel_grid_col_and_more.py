# Generated by Django 4.2 on 2023-06-24 19:11

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('projects', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='modularstoragepanel',
            name='grid_col',
            field=models.IntegerField(blank=True, null=True),
        ),
        migrations.AddField(
            model_name='modularstoragepanel',
            name='grid_row',
            field=models.IntegerField(blank=True, null=True),
        ),
        migrations.AddField(
            model_name='modularstoragepanelbase',
            name='grid_col',
            field=models.IntegerField(blank=True, null=True),
        ),
        migrations.AddField(
            model_name='modularstoragepanelbase',
            name='grid_row',
            field=models.IntegerField(blank=True, null=True),
        ),
    ]
