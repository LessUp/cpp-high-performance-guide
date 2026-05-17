<script setup lang="ts">
interface ReferenceLink {
  href: string
  label: string
}

interface Props {
  type: 'paper' | 'book' | 'repo' | 'article' | 'manual'
  title: string
  authors?: string
  year?: number
  venue?: string
  abstract?: string
  links?: ReferenceLink[]
}

const typeLabels: Record<string, string> = {
  paper: 'Paper',
  book: 'Book',
  repo: 'Repository',
  article: 'Article',
  manual: 'Manual',
}

defineProps<Props>()
</script>

<template>
  <article class="wp-reference-card">
    <div class="wp-reference-card-header">
      <span class="wp-reference-card-type">{{ typeLabels[type] ?? type }}</span>
    </div>

    <h4 class="wp-reference-card-title">{{ title }}</h4>

    <div class="wp-reference-card-meta">
      <span v-if="authors" class="reference-authors">{{ authors }}</span>
      <span v-if="year"> ({{ year }})</span>
      <span v-if="venue">. <em>{{ venue }}</em></span>
    </div>

    <p v-if="abstract" class="wp-reference-card-abstract">{{ abstract }}</p>

    <div v-if="links?.length" class="wp-reference-card-links">
      <BaseAwareLink
        v-for="link in links"
        :key="`${link.href}:${link.label}`"
        class="wp-pill-link"
        :href="link.href"
      >
        {{ link.label }}
      </BaseAwareLink>
    </div>
  </article>
</template>
