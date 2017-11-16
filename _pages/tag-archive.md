---
layout: archive
permalink: /tags/
title: "Surprising Minds Project Blog"
author_profile: false
sidebar: 
  nav: docs
---

This blog serves as a shared, online, open lab notebook for the Surprising Minds exhibit. Posts are organized by tag below. 

{% include group-by-array collection=site.posts field="tags" %}

{% for tag in group_names %}
  {% assign posts = group_items[forloop.index0] %}
  <h2 id="{{ tag | slugify }}" class="archive__subtitle">{{ tag }}</h2>
  <div class="grid__wrapper">
    {% for post in posts %}
      {% include archive-single.html type="grid" %}
    {% endfor %}
  </div>
{% endfor %}
