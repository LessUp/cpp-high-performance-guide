<script setup lang="ts">
interface SectionIndexLink {
  label: string
  href: string
}

interface SectionIndexItem {
  title: string
  description: string
  links?: SectionIndexLink[]
}

defineProps<{
  items: SectionIndexItem[]
}>()
</script>

<template>
  <section class="wp-section-index">
    <article
      v-for="item in items"
      :key="`${item.title}-${item.description}`"
      class="wp-section-index__item"
    >
      <h3 class="wp-section-index__title">{{ item.title }}</h3>
      <p class="wp-section-index__description">{{ item.description }}</p>
      <div v-if="item.links?.length" class="wp-section-index__links">
        <a
          v-for="link in item.links"
          :key="`${item.title}-${link.href}-${link.label}`"
          class="wp-pill-link"
          :href="link.href"
        >
          {{ link.label }}
        </a>
      </div>
    </article>
  </section>
</template>
