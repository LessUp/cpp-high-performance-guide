<script setup lang="ts">
type SectionLink = {
  href: string
  label: string
}

type SectionIndexItem = {
  title: string
  description: string
  links?: SectionLink[]
}

defineProps<{
  items: SectionIndexItem[]
}>()
</script>

<template>
  <div class="wp-section-index">
    <div class="wp-section-index-grid">
      <article
        v-for="item in items"
        :key="item.title"
        class="wp-section-card"
      >
        <h3 class="wp-section-card-title">{{ item.title }}</h3>
        <p class="wp-section-card-desc">{{ item.description }}</p>

        <div v-if="item.links?.length" class="wp-section-card-links">
          <a
            v-for="link in item.links"
            :key="`${item.title}:${link.href}:${link.label}`"
            class="wp-pill-link"
            :href="link.href"
          >
            {{ link.label }}
          </a>
        </div>
      </article>
    </div>
  </div>
</template>
